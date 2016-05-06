#include "column_list_adapter.hpp"

#include <cassert>


ColumnListAdapter::ColumnListAdapter( const QModelIndex & row )
	: model_( row.model() )
	, parent_( row.parent() )
	, row_( row.row() )
{
	assert( row.isValid() );
	connect();
}

ColumnListAdapter::~ColumnListAdapter()
{
	disconnect();
}

QAbstractItemModel * ColumnListAdapter::model() const
{
	return const_cast< QAbstractItemModel * >( model_ );
}

QModelIndex ColumnListAdapter::adaptedIndex(int row, int column, const QModelIndex &parent) const
{
	return model()->index( row_, row, parent_ );
}

int ColumnListAdapter::rowCount(const QModelIndex &parent) const
{
	return model()->columnCount( adaptedIndex( 0, 0, QModelIndex() ) );
}

void ColumnListAdapter::onParentDataChanged(const QModelIndex &topLeft, 
	const QModelIndex &bottomRight, const QVector<int> &roles)
{
	if (topLeft.row() <= row_ &&
		bottomRight.row() >= row_ &&
		topLeft.parent() == parent_ &&
		bottomRight.parent() == parent_)
	{
		emit dataChanged( 
			createIndex(topLeft.column(), 0, topLeft.internalPointer()), 
			createIndex(bottomRight.column(), 0, bottomRight.internalPointer()), 
			roles );
	}
}
