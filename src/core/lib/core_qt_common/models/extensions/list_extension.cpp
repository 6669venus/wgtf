#include "list_extension.hpp"
#include "core_qt_common/models/qt_abstract_item_model.hpp"

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

QModelIndex ListExtension::getDownOneRow( const QModelIndex & index ) const
{
	const auto pModel = dynamic_cast< const QtAbstractItemModel * >( index.model() );
	if (pModel == nullptr)
	{
		return index;
	}
	if (index.row() >= pModel->rowCount( index.parent() ))
	{
		return index;
	}
	return pModel->index( index.row() + 1, index.column(), index.parent() );
}


QModelIndex ListExtension::getUpOneRow( const QModelIndex & index ) const
{
	const auto pModel = dynamic_cast< const QtAbstractItemModel * >( index.model() );
	if (pModel == nullptr)
	{
		return index;
	}
	if (index.row() <= 0)
	{
		return index;
	}
	return pModel->index( index.row() - 1, index.column(), index.parent() );
}


int ListExtension::indexToRow( const QModelIndex & index ) const
{
	return index.row();
}
