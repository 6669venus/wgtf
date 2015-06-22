#include "selection_extension.hpp"
#include <cassert>
#include <set>

struct SelectionExtension::Implementation
{
	Implementation( SelectionExtension& self );

	void select( const QModelIndex& index );
	void selectRange( const QModelIndex& index );
	void deselect( const QModelIndex& index );
	bool selected( const QModelIndex& index );

	void fireDataChangedEvent( const QModelIndex& index );
	QModelIndex firstColumnIndex( const QModelIndex& index );
	QVector<int>& selectionRoles();

	SelectionExtension& self_;
	QPersistentModelIndex selectedIndex_;
	quintptr selectedItem_;
	bool allowMultiSelect_;
	bool selectRange_;
	std::set<QPersistentModelIndex> selection_;
	QVector<int> selectionRoles_;
};


SelectionExtension::Implementation::Implementation( SelectionExtension& self )
	: self_( self )
	, allowMultiSelect_( false )
	, selectedItem_( 0 )
	, selectRange_( false )
{
}


/**
 *	Update the selection data on every item in the row.
 *	Only change the selection if the row has changed.
 *	e.g.
 *							v- Clicked on Item(0,2)
 *	+---------+---------+---------+---------+
 *	|Item(0,0)|Item(0,1)|Item(0,2)|Item(0,3)| <- dataChanged for Items(0, 0-3)
 *	+---------+---------+---------+---------+
 *	|Item(1,0)|Item(1,1)|Item(1,2)|Item(1,3)|
 *	+---------+---------+---------+---------+
 *	e.g.
 *					v- Clicked on Item(0,1)
 *	+---------+---------+---------+---------+
 *	|Item(0,0)|Item(0,1)|Item(0,2)|Item(0,3)| <- same row selected as before,
 *	+---------+---------+---------+---------+    no dataChanged events
 *	|Item(1,0)|Item(1,1)|Item(1,2)|Item(1,3)|
 *	+---------+---------+---------+---------+
 */
void SelectionExtension::Implementation::select(
	const QModelIndex& index )
{
	assert( self_.model_ != nullptr );

	if (selectRange_)
	{
		selectRange( index );
		return;
	}

	// Always use column 0
	const auto adjustedIndex = firstColumnIndex( index );
	auto inserted = selection_.insert( adjustedIndex ).second;

	if (inserted)
	{
		if (!selectionRoles().empty())
		{
			if (!allowMultiSelect_ && selectedIndex_.isValid())
			{
				selection_.erase( selectedIndex_ );
				fireDataChangedEvent( selectedIndex_ );
			}

			if (adjustedIndex.isValid())
			{
				fireDataChangedEvent( adjustedIndex );
			}
		}

		selectedIndex_ = adjustedIndex;
		selectedItem_ = adjustedIndex.internalId();

		emit self_.selectionChanged();
	}
}


void SelectionExtension::Implementation::selectRange( const QModelIndex& index )
{
	assert( self_.model_ != nullptr );

	// Always use column 0
	QModelIndex toIndex = firstColumnIndex( index );
	QModelIndex fromIndex =
		selectedIndex_.isValid() ? selectedIndex_ :
		toIndex;

	assert( toIndex.parent() == fromIndex.parent() );
	selectedIndex_ = fromIndex;
	selectRange_ = false;

	if (toIndex.isValid())
	{
		if (!selectionRoles().empty())
		{
			if (toIndex.row() < fromIndex.row())
			{
				std::swap( toIndex, fromIndex );
			}

			decltype(selection_) oldSelection;
			oldSelection.swap( selection_ );

			for (int i = fromIndex.row(); i <= toIndex.row(); ++i)
			{
				QModelIndex newIndex = fromIndex.sibling( i, 0 );
				selection_.insert( newIndex );

				if (oldSelection.erase( newIndex ) == 0)
				{
					fireDataChangedEvent( newIndex );
				}
			}

			for (auto& oldIndex: oldSelection)
			{
				fireDataChangedEvent( oldIndex );
			}
		}

		emit self_.selectionChanged();
	}
}


void SelectionExtension::Implementation::deselect( const QModelIndex& index )
{
	assert( self_.model_ != nullptr );
	assert( index.isValid() );

	// Always use column 0
	const auto adjustedIndex = firstColumnIndex( index );
	auto removedIndices = selection_.erase( adjustedIndex );
	selectRange_ = false;

	if (removedIndices > 0)
	{
		selectedIndex_ = QModelIndex();

		if (!selectionRoles().empty())
		{
			fireDataChangedEvent( adjustedIndex );
		}

		emit self_.selectionChanged();
	}
}


/**
 *	Selection extension can only select whole rows.
 *	Do not compare columns, because that would select only one part of a row.
 */
bool SelectionExtension::Implementation::selected( const QModelIndex& index )
{
	if (!index.isValid())
	{
		return false;
	}

	// Always use column 0
	const auto adjustedIndex = firstColumnIndex( index );
	return selection_.find( adjustedIndex ) != selection_.end();
}


void SelectionExtension::Implementation::fireDataChangedEvent(
	const QModelIndex& index )
{
	const auto columnCount = self_.model_->columnCount( index );
	assert( columnCount > 0 );
	const auto lastColumn = columnCount - 1;

	const auto topLeft = index;
	const auto bottomRight =
		self_.model_->sibling( index.row(), lastColumn, index );
	assert( bottomRight.isValid() );

	emit self_.model_->dataChanged( topLeft, bottomRight, selectionRoles() );
}


QModelIndex SelectionExtension::Implementation::firstColumnIndex(
	const QModelIndex& index )
{
	return
		!index.isValid() ? QModelIndex() :
		index.row() == 0 ? index :
		self_.model_->sibling( index.row(), 0, index );
}


QVector<int>& SelectionExtension::Implementation::selectionRoles()
{
	if (selectionRoles_.empty())
	{
		int role;

		if (self_.encodeRole( SelectedRole::roleId_, role ))
		{
			selectionRoles_.append( role );
		}
	}

	return selectionRoles_;
}


SelectionExtension::SelectionExtension()
	: impl_( new Implementation( *this ) )
{
}


SelectionExtension::~SelectionExtension()
{
}


QHash< int, QByteArray > SelectionExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	this->registerRole( SelectedRole::role_, roleNames );
	return roleNames;
}


QVariant SelectionExtension::data( const QModelIndex &index, int role ) const
{
	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId != SelectedRole::roleId_)
	{
		return QVariant( QVariant::Invalid );
	}

	return impl_->selected( index );
}


bool SelectionExtension::setData( 
	const QModelIndex &index, const QVariant &value, int role )
{
	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return false;
	}

	if (roleId != SelectedRole::roleId_)
	{
		return false;
	}

	if (value.toBool())
	{
		impl_->select( index );
	}
	else
	{
		impl_->deselect( index );
	}

	return true;
}


void SelectionExtension::onDataAboutToBeChanged( const QModelIndex& index,
	int role,
	const QVariant& value )
{
	// Does nothing
}


void SelectionExtension::onDataChanged( const QModelIndex& index,
	int role,
	const QVariant& value )
{
	// Does nothing
}


void SelectionExtension::clearSelection( bool keepLastSelectedIndex )
{
	if (!keepLastSelectedIndex)
	{
		impl_->selectedIndex_ = QModelIndex();
	}

	decltype(impl_->selection_) oldSelection;
	oldSelection.swap( impl_->selection_ );

	if (!impl_->selectionRoles().empty())
	{
		for (auto& index: oldSelection)
		{
			if (index != impl_->selectedIndex_)
			{
				impl_->fireDataChangedEvent( index );
			}
		}
	}

	if (impl_->selectedIndex_.isValid())
	{
		impl_->selection_.insert( impl_->selectedIndex_ );
	}

	emit selectionChanged();
}


void SelectionExtension::prepareRangeSelect()
{
	impl_->selectRange_ = true;
}


QVariant SelectionExtension::getSelectedIndex() const
{
	QModelIndex index = impl_->selectedIndex_;
	return QVariant::fromValue( index );
}

QVariant SelectionExtension::getSelectedItem() const
{
	return QVariant::fromValue( impl_->selectedItem_ );
}

void SelectionExtension::setSelectedIndex( const QVariant& index )
{
	QModelIndex idx = index.toModelIndex();
	impl_->select( idx );
}


bool SelectionExtension::getMultiSelect() const
{
	return impl_->allowMultiSelect_;
}


void SelectionExtension::setMultiSelect( bool value )
{
	impl_->allowMultiSelect_ = value;
	emit multiSelectChanged();

	if (!value && impl_->selection_.size() > 1)
	{
		clearSelection( true );
	}
}
