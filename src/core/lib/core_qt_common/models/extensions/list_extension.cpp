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

	if (first.row() > last.row())
	{
		return QItemSelection( last, first );
	}
	return QItemSelection( first, last );
}