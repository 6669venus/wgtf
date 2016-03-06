#include "qt_item_model.hpp"

#include "core_data_model/abstract_item_model.hpp"
#include "helpers/qt_helpers.hpp"

#include <QUuid>

#include <private/qmetaobjectbuilder_p.h>

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

				metaObject_.reset( builder.toMetaObject() );
			}

			QAbstractItemModel & model_;
			QList< int > roles_;
			std::unique_ptr< QMetaObject > metaObject_;
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

			return metaObject->metaObject_.get();
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

AbstractItemModel & QtItemModel::source() const
{
	return impl_->source_;
}

QObject * QtItemModel::item( int row, int column, const QObject * parent ) const
{
	auto parentItem = qobject_cast< const ItemData * >( parent );
	auto parentIndex = parent != nullptr ? parentItem->index_ : QModelIndex();
	auto index = this->index( row, column, parentIndex );
	return new ItemData( index, impl_->metaObject_ );
}

int QtItemModel::rowCount( const QObject * parent ) const
{
	auto parentItem = qobject_cast< const ItemData * >( parent );
	auto parentIndex = parent != nullptr ? parentItem->index_ : QModelIndex();
	return rowCount( parentIndex );
}

int QtItemModel::columnCount( const QObject * parent ) const
{
	auto parentItem = qobject_cast< const ItemData * >( parent );
	auto parentIndex = parent != nullptr ? parentItem->index_ : QModelIndex();
	return columnCount( parentIndex );
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

	auto data = item->getData( index.row(), index.column(), role );
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

AbstractListModel & QtListModel::source() const
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

QtTreeModel::QtTreeModel( AbstractTreeModel & source ) 
	: QtItemModel( source ) 
{}

AbstractTreeModel & QtTreeModel::source() const
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

QtTableModel::QtTableModel( AbstractTableModel & source ) 
	: QtItemModel( source ) 
{}

AbstractTableModel & QtTableModel::source() const
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