#include "selection_extension.hpp"
#include <cassert>
#include <set>

struct SelectionExtension::Implementation
{
	Implementation( SelectionExtension& self );

	QModelIndex findNextIndex(
		const QAbstractItemModel* model, const QModelIndex& index, bool skipChildren = false ) const;
	std::vector<QModelIndex> findRange(
		const QAbstractItemModel* model, const QModelIndex& from, const QModelIndex& to ) const;

	void select( const QModelIndex& index );
	void selectRange( const QModelIndex& index );
	void deselect( const QModelIndex& index );
	bool selected( const QModelIndex& index );

	void fireDataChangedEvent( const QModelIndex& index );
	QModelIndex firstColumnIndex( const QModelIndex& index );
	QVector<int>& selectionRoles();
	int expandedRole() const;

	QModelIndex lastSelectedIndex() const;

	SelectionExtension& self_;
	QPersistentModelIndex lastClickedIndex_;
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


QModelIndex SelectionExtension::Implementation::findNextIndex(
	const QAbstractItemModel* model, const QModelIndex& index, bool skipChildren ) const
{
	if (index.isValid())
	{
		QModelIndex next;

		if (!skipChildren)
		{
			bool expanded = model->data( index, expandedRole() ).toBool();

			if (expanded)
			{
				next = index.child( 0, 0 );

				if (next.isValid())
				{
					return next;
				}
			}
		}

		next = index.parent();
		int nextRow = index.row() + 1;

		if (nextRow < model->rowCount( next ))
		{
			return index.sibling( nextRow, 0 );
		}
		else if (next.isValid())
		{
			return findNextIndex( model, next, true );
		}
	}

	return QModelIndex();
}


std::vector<QModelIndex> SelectionExtension::Implementation::findRange(
	const QAbstractItemModel* model, const QModelIndex& from, const QModelIndex& to ) const
{
	QModelIndex position = model->index( 0, 0, QModelIndex() );
	std::vector<QModelIndex> indices;
	bool inRange = false;

	for (; position.isValid(); position = findNextIndex( model, position ))
	{
		if (position == from || position == to)
		{
			indices.push_back( position );

			if (inRange || from == to)
			{
				inRange = false;
				break;
			}

			inRange = true;
		}
		else if (inRange)
		{
			indices.push_back( position );
		}
	}

	if (inRange)
	{
		indices.clear();
	}

	return indices;
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
			if (!allowMultiSelect_ && lastClickedIndex_.isValid())
			{
				selection_.erase( lastClickedIndex_ );
				fireDataChangedEvent( lastClickedIndex_ );
			}

			if (adjustedIndex.isValid())
			{
				fireDataChangedEvent( adjustedIndex );
			}
		}
	}

	lastClickedIndex_ = adjustedIndex;
	selectedItem_ = adjustedIndex.internalId();

	if (inserted)
	{
		emit self_.selectionChanged();
	}
}


void SelectionExtension::Implementation::selectRange( const QModelIndex& index )
{
	assert( self_.model_ != nullptr );
	assert( index.isValid() );

	// Always use column 0
	QModelIndex toIndex = firstColumnIndex( index );
	QModelIndex fromIndex = lastClickedIndex_;
	bool fromIndexSelected = lastSelectedIndex().isValid();
	selectRange_ = false;

	if (!lastClickedIndex_.isValid())
	{
		fromIndex = toIndex;
		lastClickedIndex_ = fromIndex;
	}

	if (!selectionRoles().empty())
	{
		const QAbstractItemModel* model = index.model();
		std::vector<QModelIndex> range = findRange( model, fromIndex, toIndex );

		if (fromIndexSelected)
		{
			selection_.erase( fromIndex );
		}

		decltype(selection_) oldSelection;
		oldSelection.swap( selection_ );

		for (auto& rangeIndex: range)
		{
			selection_.insert( rangeIndex );

			if (oldSelection.erase( rangeIndex ) == 0 && (rangeIndex != fromIndex || !fromIndexSelected))
			{
				fireDataChangedEvent( rangeIndex );
			}
		}

		for (auto& oldIndex: oldSelection)
		{
			fireDataChangedEvent( oldIndex );
		}
	}

	emit self_.selectionChanged();
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


int SelectionExtension::Implementation::expandedRole() const
{
	static int expandedRole = -1;
	
	if (expandedRole < 0)
	{
		self_.encodeRole( ExpandedRole::roleId_, expandedRole );
	}

	return expandedRole;
}


QModelIndex SelectionExtension::Implementation::lastSelectedIndex() const
{
	if (selection_.find( lastClickedIndex_ ) != selection_.cend())
	{
		return lastClickedIndex_;
	}

	return QModelIndex();
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
	this->registerRole( ExpandedRole::role_, roleNames );
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
		impl_->lastClickedIndex_ = QModelIndex();
	}

	QModelIndex lastSelectedIndex = impl_->lastSelectedIndex();
	decltype(impl_->selection_) oldSelection;
	oldSelection.swap( impl_->selection_ );

	if (!impl_->selectionRoles().empty())
	{
		for (auto& index: oldSelection)
		{
			if (index != lastSelectedIndex && index.isValid())
			{
				impl_->fireDataChangedEvent( index );
			}
		}
	}

	if (lastSelectedIndex.isValid())
	{
		impl_->selection_.insert( lastSelectedIndex );
	}

	emit selectionChanged();
}


void SelectionExtension::prepareRangeSelect()
{
	impl_->selectRange_ = true;
}


QVariant SelectionExtension::getSelectedIndex() const
{
	return QVariant::fromValue( impl_->lastSelectedIndex() );
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
