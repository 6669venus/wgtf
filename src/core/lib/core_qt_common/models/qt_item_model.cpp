#include "qt_item_model.hpp"

#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "helpers/qt_helpers.hpp"

ITEMROLE( display )
ITEMROLE( decoration )

struct QtItemModel::Impl
{
	Impl( AbstractItemModel & source )
		: source_( source )
	{
	}

	AbstractItemModel & source_;

	Connection connectPreChange_;
	Connection connectPostChanged_;

	Connection connectPreInsert_;
	Connection connectPostInserted_;

	Connection connectPreErase_;
	Connection connectPostErase_;
};

QtItemModel::QtItemModel( AbstractItemModel & source )
	: impl_( new Impl( source ) )
{
	// @see AbstractItemModel::DataSignature
	auto preData = 
	[ this ]( const AbstractItemModel::ItemIndex & index,
		size_t role,
		const Variant & newValue )
	{
		auto item = impl_->source_.item( index );
		const QModelIndex modelIndex = createIndex( index.row_, index.column_, item );

		//HACK: should not be explicitly referencing DefinitionRole here
		if (role == DefinitionRole::roleId_)
		{
			QList<QPersistentModelIndex> parents;
			parents.append( modelIndex );
			this->layoutAboutToBeChanged( parents, QAbstractItemModel::VerticalSortHint );
			return;
		}
		//END HACK
	};
	impl_->connectPreChange_ =
		impl_->source_.connectPreItemDataChanged( preData );
	
	auto postData = 
	[ this ]( const AbstractItemModel::ItemIndex & index,
		size_t role,
		const Variant & newValue )
	{
		auto item = impl_->source_.item( index );
		const QModelIndex modelIndex = createIndex( index.row_, index.column_, item );

		//HACK: should not be explicitly referencing DefinitionRole here
		if (role == DefinitionRole::roleId_)
		{
			QList<QPersistentModelIndex> parents;
			parents.append( modelIndex );
			this->layoutChanged( parents, QAbstractItemModel::VerticalSortHint );
			return;
		}
		//END HACKs

		const QModelIndex topLeft = modelIndex;
		const QModelIndex bottomRight = modelIndex;
		QVector< int > roles;
		if (role == ItemRole::displayId)
		{
			roles.append( Qt::DisplayRole );
		}
		else if (role == ItemRole::decorationId)
		{
			roles.append( Qt::DecorationRole );
		}
		else
		{
			roles.append( static_cast< int >( role ) );
		}
		this->dataChanged( topLeft, bottomRight, roles );
	};
	impl_->connectPostChanged_ =
		impl_->source_.connectPostItemDataChanged( postData );
}

QtItemModel::~QtItemModel()
{
	// TODO not removed from list??
	impl_->connectPostChanged_.disconnect();
	impl_->connectPreChange_.disconnect();
}

const AbstractItemModel & QtItemModel::source() const
{
	return impl_->source_;
}

AbstractItemModel & QtItemModel::source()
{
	return impl_->source_;
}

QModelIndex QtItemModel::index( int row, int column, const QModelIndex &parent ) const
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr;

	AbstractItemModel::ItemIndex itemIndex( row, column, parentItem );
	auto item = impl_->source_.item( itemIndex );
	if (item == nullptr)
	{
		return QModelIndex();
	}

	return createIndex( row, column, item );
}

QModelIndex QtItemModel::parent( const QModelIndex &child ) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}

	auto childItem = reinterpret_cast< AbstractItem * >( child.internalId() );
	AbstractItemModel::ItemIndex childIndex;
	impl_->source_.index( childItem, childIndex );
	if (!childIndex.isValid())
	{
		return QModelIndex();
	}

	auto parentItem = const_cast< AbstractItem * >( childIndex.parent_ );
	if (parentItem == nullptr)
	{
		return QModelIndex();
	}

	AbstractItemModel::ItemIndex parentIndex;
	impl_->source_.index( parentItem, parentIndex );
	if (!parentIndex.isValid())
	{
		return QModelIndex();
	}

	return createIndex( parentIndex.row_, parentIndex.column_, parentItem );
}

int QtItemModel::rowCount( const QModelIndex &parent ) const
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr;

	return impl_->source_.rowCount( parentItem );
}

int QtItemModel::columnCount( const QModelIndex &parent ) const
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr;

	return impl_->source_.columnCount( parentItem );
}

QVariant QtItemModel::data( const QModelIndex &index, int role ) const
{
	auto item = index.isValid() ?
		reinterpret_cast< AbstractItem * >( index.internalId() ) : nullptr; 

	if (item == nullptr)
	{
		return QVariant();
	}
	
	size_t roleId;
	switch (role)
	{
	case Qt::DisplayRole:
		roleId = ItemRole::displayId;
		break;

	case Qt::DecorationRole:
		roleId = ItemRole::decorationId;
		break;

	default:
		roleId = role;
		break;
	}

	auto data = item->getData( index.row(), index.column(), roleId );
	return QtHelpers::toQVariant( data, const_cast<QtItemModel*>(this) );
}

bool QtItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
	auto item = index.isValid() ?
		reinterpret_cast< AbstractItem * >( index.internalId() ) : nullptr; 

	if (item == nullptr)
	{
		return false;
	}

	auto oldValue = QtHelpers::toQVariant( item->getData( index.row(), index.column(), role ), this );
	if (value == oldValue)
	{
		return true;
	}

	auto data = QtHelpers::toVariant( value );
	return item->setData( index.row(), index.column(), role, data );
}

QVariant QtItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	auto row = orientation == Qt::Horizontal ? section : -1;
	auto column = orientation == Qt::Vertical ? section : -1;
	auto data = impl_->source_.getData( row, column, role );
	return QtHelpers::toQVariant( data, nullptr );
}

bool QtItemModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role )
{
	auto row = orientation == Qt::Horizontal ? section : -1;
	auto column = orientation == Qt::Vertical ? section : -1;
	auto data = QtHelpers::toVariant( value );
	return impl_->source_.setData( row, column, role, data );
}

QtListModel::QtListModel( AbstractListModel & source ) 
	: QtItemModel( source ) 
{}

const AbstractListModel & QtListModel::source() const
{
	return static_cast< const AbstractListModel & >( QtItemModel::source() );
}

AbstractListModel & QtListModel::source()
{
	return static_cast< AbstractListModel & >( QtItemModel::source() );
}

QObject * QtListModel::item( int row ) const 
{
	return QtAbstractItemModel::item( row, 0, nullptr ); 
}

int QtListModel::count() const 
{ 
	return QtAbstractItemModel::rowCount( nullptr ); 
}


bool QtListModel::insertItem( int row )
{
	return QtAbstractItemModel::insertRow( row, nullptr ); 
}


bool QtListModel::removeItem( int row )
{
	return QtAbstractItemModel::removeRow( row, nullptr ); 
}


QtTreeModel::QtTreeModel( AbstractTreeModel & source ) 
	: QtItemModel( source ) 
{}

const AbstractTreeModel & QtTreeModel::source() const
{
	return static_cast< const AbstractTreeModel & >( QtItemModel::source() );
}

AbstractTreeModel & QtTreeModel::source()
{
	return static_cast< AbstractTreeModel & >( QtItemModel::source() );
}

QObject * QtTreeModel::item( int row, QObject * parent ) const 
{ 
	return QtAbstractItemModel::item( row, 0, parent ); 
}

int QtTreeModel::count( QObject * parent ) const 
{ 
	return QtAbstractItemModel::rowCount( parent ); 
}


bool QtTreeModel::insertItem( int row, QObject * parent )
{
	return QtAbstractItemModel::insertRow( row, parent ); 
}


bool QtTreeModel::removeItem( int row, QObject * parent )
{
	return QtAbstractItemModel::removeRow( row, parent ); 
}


QtTableModel::QtTableModel( AbstractTableModel & source ) 
	: QtItemModel( source ) 
{}

const AbstractTableModel & QtTableModel::source() const
{
	return static_cast< const AbstractTableModel & >( QtItemModel::source() );
}

AbstractTableModel & QtTableModel::source()
{
	return static_cast< AbstractTableModel & >( QtItemModel::source() );
}

QObject * QtTableModel::item( int row, int column ) const 
{
	return QtAbstractItemModel::item( row, column, nullptr ); 
}

int QtTableModel::rowCount() const 
{ 
	return QtAbstractItemModel::rowCount( nullptr ); 
}

int QtTableModel::columnCount() const 
{ 
	return QtAbstractItemModel::columnCount( nullptr ); 
}


bool QtTableModel::insertRow( int row )
{
	return QtAbstractItemModel::insertRow( row, nullptr ); 
}


bool QtTableModel::insertColumn( int column )
{
	return QtAbstractItemModel::insertColumn( column, nullptr ); 
}


bool QtTableModel::removeRow( int row )
{
	return QtAbstractItemModel::removeRow( row, nullptr ); 
}


bool QtTableModel::removeColumn( int column )
{
	return QtAbstractItemModel::removeColumn( column, nullptr ); 
}
