#include "list_extension.hpp"

ListExtension::ListExtension()
{

}

ListExtension::~ListExtension()
{

}

QHash< int, QByteArray > ListExtension::roleNames() const
{
	return QHash< int, QByteArray >();
}

QItemSelection ListExtension::itemSelection( const QModelIndex & first, const QModelIndex & last ) const
{
	if (!first.isValid() && !last.isValid())
	{
		return QItemSelection();
	}
	if (!first.isValid() && last.isValid())
	{
		return QItemSelection( last, last );
	}
	if (first.isValid() && !last.isValid())
	{
		return QItemSelection( first, first );
	}

	assert( first.column() == 0 && !first.parent().isValid() );
	assert( last.column() == 0 && !last.parent().isValid() );

	auto begin = first;
	auto end = last;

	if (begin.row() > end.row())
	{
		std::swap(begin, end);
	}

	return QItemSelection( begin, end );
}


QModelIndex ListExtension::getRowToIndex( const QModelIndex & index,
	const int row ) const
{
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}
	return pModel->index( row, index.column(), index.parent() );
}


int ListExtension::getIndexToRow( const QModelIndex & index ) const
{
	return index.row();
}
