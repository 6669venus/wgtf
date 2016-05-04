#include "qt_item_model.hpp"

#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "helpers/qt_helpers.hpp"

#include <QUuid>

#include <private/qmetaobjectbuilder_p.h>

ITEMROLE( display )
ITEMROLE( decoration )

namespace
{
	class ItemData : public QObject
	{
	public:
		struct MetaObject
		{
			MetaObject( QAbstractItemModel & model )
				: model_( model )
			{
				QMetaObjectBuilder builder;
				builder.setClassName( QUuid().toByteArray() );
				builder.setSuperClass( &QObject::staticMetaObject );

				QHashIterator<int, QByteArray> itr( model_.roleNames() );
				while (itr.hasNext())
				{
					itr.next();
					roles_.append( itr.key() );
					auto property = builder.addProperty( itr.value(), "QVariant" );
					property.setNotifySignal( builder.addSignal( itr.value() + "Changed(QVariant)" ) );
				}

				metaObject_ = builder.toMetaObject();
			}

			virtual ~MetaObject()
			{
				// @see QMetaObjectBuilder::toMetaObject()
				// "The return value should be deallocated using free() once it
				// is no longer needed."
				// Allocation was done by the Qt dll, so use global free()
				// Do not use the NGT allocator
				::free( metaObject_ );
			}

			QAbstractItemModel & model_;
			QList< int > roles_;
			QMetaObject * metaObject_;
		};

		ItemData( const QModelIndex & index, std::weak_ptr< MetaObject > metaObject )
			: index_( index )
			, metaObject_( metaObject )
		{
		}

		const QMetaObject * metaObject() const override
		{
			auto metaObject = metaObject_.lock();
			if (metaObject == nullptr)
			{
				return nullptr;
			}

			return metaObject->metaObject_;
		}

		int qt_metacall( QMetaObject::Call c, int id, void **argv ) override
		{
			auto metaObject = metaObject_.lock();
			if (metaObject == nullptr)
			{
				return -1;
			}

			id = QObject::qt_metacall( c, id, argv );
			if (id < 0)
			{
				return id;
			}

			switch (c) 
			{
			case QMetaObject::InvokeMetaMethod:
				{
					auto methodCount = metaObject->metaObject_->methodCount() - metaObject->metaObject_->methodOffset();
					if (id < methodCount)
					{
						metaObject->metaObject_->activate( this, id + metaObject->metaObject_->methodOffset(), argv );
					}
					id -= methodCount;
					break;
				}
			case QMetaObject::ReadProperty:
			case QMetaObject::WriteProperty:
				{
					auto propertyCount = metaObject->metaObject_->propertyCount() - metaObject->metaObject_->propertyOffset();
					if (id < propertyCount)
					{
						auto value = reinterpret_cast< QVariant * >( argv[0] );
						auto role = metaObject->roles_[id];
						if (c == QMetaObject::ReadProperty)
						{
							*value = metaObject->model_.data( index_, role );
						}
						else
						{
							metaObject->model_.setData( index_, *value, role );
						}
					}
					id -= propertyCount;
					break;
				}
			default:
				break;
			}

			return id;
		}

		QModelIndex index_;
		std::weak_ptr< MetaObject > metaObject_;
	};


	/**
	 *	Helper to convert item to model index.
	 */
	QModelIndex itemToIndex( const QObject * item )
	{
		auto itemData = qobject_cast< const ItemData * >( item );
		return item != nullptr ? itemData->index_ : QModelIndex();
	}
}

struct QtItemModel::Impl
{
	Impl( AbstractItemModel & source )
		: source_( source )
	{
	}

	AbstractItemModel & source_;
	std::shared_ptr< ItemData::MetaObject > metaObject_;
};

QtItemModel::QtItemModel( AbstractItemModel & source )
	: impl_( new Impl( source ) )
{
	impl_->metaObject_.reset( new ItemData::MetaObject( *this ) );
	QObject::connect( this, &QAbstractItemModel::modelReset, [&]() 
	{ 
		impl_->metaObject_.reset( new ItemData::MetaObject( *this ) ); 
	});
}

QtItemModel::~QtItemModel()
{
}

const AbstractItemModel & QtItemModel::source() const
{
	return impl_->source_;
}

AbstractItemModel & QtItemModel::source()
{
	return impl_->source_;
}

QObject * QtItemModel::item( int row, int column, const QObject * parent ) const
{
	auto parentIndex = itemToIndex( parent );
	auto index = this->index( row, column, parentIndex );
	return new ItemData( index, impl_->metaObject_ );
}

int QtItemModel::rowCount( const QObject * parent ) const
{
	auto parentIndex = itemToIndex( parent );
	return rowCount( parentIndex );
}

int QtItemModel::columnCount( const QObject * parent ) const
{
	auto parentIndex = itemToIndex( parent );
	return columnCount( parentIndex );
}

bool QtItemModel::insertRow( int row, const QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::insertRow( row, parentIndex );
}

bool QtItemModel::insertColumn( int column, const QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::insertColumn( column, parentIndex );
}

bool QtItemModel::removeRow( int row, const QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::removeRow( row, parentIndex );
}

bool QtItemModel::removeColumn( int column, const QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::removeColumn( column, parentIndex );
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
	return QtHelpers::toQVariant( data );
}

bool QtItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
	auto item = index.isValid() ?
		reinterpret_cast< AbstractItem * >( index.internalId() ) : nullptr; 

	if (item == nullptr)
	{
		return false;
	}

	auto oldValue = QtHelpers::toQVariant( item->getData( index.row(), index.column(), role ) );
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
	return QtHelpers::toQVariant( data );
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
	return QtItemModel::item( row, 0, nullptr ); 
}

int QtListModel::count() const 
{ 
	return QtItemModel::rowCount( nullptr ); 
}


bool QtListModel::insertItem( int row )
{
	return QtItemModel::insertRow( row, nullptr ); 
}


bool QtListModel::removeItem( int row )
{
	return QtItemModel::removeRow( row, nullptr ); 
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
	return QtItemModel::item( row, 0, parent ); 
}

int QtTreeModel::count( QObject * parent ) const 
{ 
	return QtItemModel::rowCount( parent ); 
}


bool QtTreeModel::insertItem( int row, QObject * parent )
{
	return QtItemModel::insertRow( row, parent ); 
}


bool QtTreeModel::removeItem( int row, QObject * parent )
{
	return QtItemModel::removeRow( row, parent ); 
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
	return QtItemModel::item( row, column, nullptr ); 
}

int QtTableModel::rowCount() const 
{ 
	return QtItemModel::rowCount( nullptr ); 
}

int QtTableModel::columnCount() const 
{ 
	return QtItemModel::columnCount( nullptr ); 
}


bool QtTableModel::insertRow( int row )
{
	return QtItemModel::insertRow( row, nullptr ); 
}


bool QtTableModel::insertColumn( int column )
{
	return QtItemModel::insertColumn( column, nullptr ); 
}


bool QtTableModel::removeRow( int row )
{
	return QtItemModel::removeRow( row, nullptr ); 
}


bool QtTableModel::removeColumn( int column )
{
	return QtItemModel::removeColumn( column, nullptr ); 
}
