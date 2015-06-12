#include "selection_extension.hpp"
#include <cassert>


struct SelectionExtension::Implementation
{
	Implementation( SelectionExtension& self );

	void select( const QModelIndex& index );
	bool selected( const QModelIndex& index );

	SelectionExtension& self_;
	QPersistentModelIndex selected_;
};


SelectionExtension::Implementation::Implementation( SelectionExtension& self )
	: self_( self )
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
void SelectionExtension::Implementation::select( const QModelIndex& index )
{
	assert( self_.model_ != nullptr );

	// Always use column 0
	const auto adjustedIndex = index.row() == 0 ? 
		index : self_.model_->sibling( index.row(), 0, index );

	const auto deselected = selected_;
	selected_ = selected_ != adjustedIndex ? adjustedIndex : QModelIndex();

	if (deselected == selected_)
	{
		return;
	}

	int role;
	if (!self_.encodeRole( SelectedRole::roleId_, role ))
	{
		return;
	}

	QVector< int > roles;
	roles.append( role );

	if (deselected.isValid())
	{
		const auto columnCount = self_.model_->columnCount( deselected );
		assert( columnCount > 0 );
		const auto lastColumn = columnCount - 1;

		const auto topLeft = deselected;
		const auto bottomRight = self_.model_->sibling(
			deselected.row(), lastColumn, deselected );
		assert( bottomRight.isValid() );
		emit self_.model_->dataChanged( topLeft, bottomRight, roles );
	}

	if (selected_.isValid())
	{
		const auto columnCount = self_.model_->columnCount( selected_ );
		assert( columnCount > 0 );
		const auto lastColumn = columnCount - 1;

		const auto topLeft = selected_;
		const auto bottomRight = self_.model_->sibling(
			selected_.row(), lastColumn, selected_ );
		assert( bottomRight.isValid() );
		emit self_.model_->dataChanged( topLeft, bottomRight, roles );
	}

	if (deselected.isValid() || selected_.isValid())
	{
		emit self_.selectionChanged();
	}
}


/**
 *	Selection extension can only select whole rows.
 *	Do not compare columns, because that would select only one part of a row.
 */
bool SelectionExtension::Implementation::selected( const QModelIndex& index )
{
	// Always use column 0
	const auto adjustedIndex = index.row() == 0 ? 
		index : self_.model_->sibling( index.row(), 0, index );
	return (selected_ == adjustedIndex);
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

	impl_->select( index );
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


QVariant SelectionExtension::getSelectedIndex() const
{
	QModelIndex index = impl_->selected_;
	return QVariant::fromValue( index );
}


void SelectionExtension::setSelectedIndex( const QVariant& index )
{
	QModelIndex idx = index.toModelIndex();

	if (idx == QModelIndex() || !impl_->selected( idx ))
	{
		impl_->select( idx );
	}
}
