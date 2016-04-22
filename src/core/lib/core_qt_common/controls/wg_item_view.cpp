#include "wg_item_view.hpp"
#include "qt_connection_holder.hpp"
#include "models/extensions/i_model_extension.hpp"

#include <QAbstractItemModel>
#include <QQmlListProperty>
#include <QString>
#include <QUuid>

#include <private/qmetaobjectbuilder_p.h>

ITEMROLE( modelIndex )

namespace
{
	class ExtendedModel : public QAbstractItemModel, public RoleProvider
	{
	public:
		ExtendedModel( QStringList & roles, QList< IModelExtension * > & extensions )
			: model_( nullptr )
			, roles_( roles )
			, extensions_( extensions )
		{
		}

		void reset( QAbstractItemModel * model )
		{
			beginResetModel();
			model_ = model;
			roleNames_.clear();
			if (model_ != nullptr)
			{
				roleNames_ = model_->roleNames();
				registerRole( ItemRole::modelIndexName, roleNames_ );
				for (auto & role : roles_)
				{
					registerRole( role.toUtf8(), roleNames_ );
				}
				for (auto & extension : extensions_)
				{
					QHashIterator<int, QByteArray> itr( extension->roleNames() );
					while (itr.hasNext())
					{
						itr.next();
						registerRole( itr.value(), roleNames_ );
					}
				}
			}
			endResetModel();
		}

	private:
		QModelIndex index( int row, int column, const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return QModelIndex();
			}

			return extendedIndex( model_->index( row, column, parent ) );
		}

		QModelIndex parent( const QModelIndex &child ) const override
		{
			if (model_ == nullptr)
			{
				return QModelIndex();
			}

			return extendedIndex( model_->parent( child ) );
		}

		int rowCount( const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return 0;
			}

			return model_->rowCount( modelIndex( parent ) );
		}

		int columnCount( const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return 0;
			}

			return model_->columnCount( modelIndex( parent ) );
		}

		bool hasChildren( const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return false;
			}
			
			return model_->hasChildren( modelIndex( parent ) );
		}

		QVariant data( const QModelIndex &index, int role ) const override
		{
			if (model_ == nullptr)
			{
				return QVariant();
			}

			for (auto & extension : extensions_)
			{
				auto data = extension->data( index, role );
				if (data.isValid())
				{
					return data;
				}
			}

			size_t roleId;
			if (decodeRole( role, roleId ))
			{
				if (roleId == ItemRole::modelIndexId)
				{
					return modelIndex( index );
				}
				role = static_cast< int >( roleId );
			}

			return model_->data( modelIndex( index ), role );
		}

		bool setData( const QModelIndex &index, const QVariant &value, int role ) override
		{
			if (model_ == nullptr)
			{
				return false;
			}
			
			for (auto & extension : extensions_)
			{
				if (extension->setData( index, value, role ) )
				{
					return true;
				}
			}

			return model_->setData( modelIndex( index ), value, role );
		}

		QVariant headerData( int section, Qt::Orientation orientation, int role ) const override
		{
			if (model_ == nullptr)
			{
				return QVariant();
			}

			for (auto & extension : extensions_)
			{
				auto data = extension->headerData( section, orientation, role );
				if (data.isValid())
				{
					return data;
				}
			}

			return model_->headerData( section, orientation, role );
		}

		bool setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role ) override
		{
			if (model_ == nullptr)
			{
				return false;
			}

			for (auto & extension : extensions_)
			{
				if (extension->setHeaderData( section, orientation, value, role ) )
				{
					return true;
				}
			}

			return model_->setHeaderData( section, orientation, value, role );
		}

		QHash< int, QByteArray > roleNames() const override
		{
			return roleNames_;
		}

		QModelIndex modelIndex( const QModelIndex & extendedIndex ) const
		{
			if (!extendedIndex.isValid())
			{
				return QModelIndex();
			}

			assert( extendedIndex.model() == this );
			auto it = indexCache_.find( extendedIndex );
			assert( it != indexCache_.end() );
			return *it;
		}

		QModelIndex extendedIndex( const QModelIndex & modelIndex ) const
		{
			if (!modelIndex.isValid())
			{
				return QModelIndex();
			}

			assert( modelIndex.model() == model_ );
			QModelIndex index = createIndex( modelIndex.row(), modelIndex.column(), modelIndex.internalId() );
			auto it = indexCache_.find( index );
			if (it != indexCache_.end())
			{
				assert( *it == modelIndex );
			}
			else
			{
				indexCache_.insert( index, modelIndex );
			}
			return index;
		}

		QAbstractItemModel * model_;
		QStringList & roles_;
		QList< IModelExtension * > & extensions_;
		QHash< int, QByteArray > roleNames_;
		mutable QHash< QPersistentModelIndex, QPersistentModelIndex > indexCache_;
	};

	class HeaderData : public QObject
	{
	public:
		HeaderData( QAbstractItemModel & model, int section, Qt::Orientation orientation )
			: model_( model )
			, section_( section )
			, orientation_( orientation )
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

	private:
		const QMetaObject * metaObject() const override
		{
			return metaObject_.get();
		}

		int qt_metacall( QMetaObject::Call c, int id, void **argv ) override
		{
			id = QObject::qt_metacall( c, id, argv );
			if (id < 0)
			{
				return id;
			}

			switch (c) 
			{
			case QMetaObject::InvokeMetaMethod:
				{
					auto methodCount = metaObject_->methodCount() - metaObject_->methodOffset();
					if (id < methodCount)
					{
						metaObject_->activate( this, id + metaObject_->methodOffset(), argv );
					}
					id -= methodCount;
					break;
				}
			case QMetaObject::ReadProperty:
			case QMetaObject::WriteProperty:
				{
					auto propertyCount = metaObject_->propertyCount() - metaObject_->propertyOffset();
					if (id < propertyCount)
					{
						auto value = reinterpret_cast< QVariant * >( argv[0] );
						auto role = roles_[id];
						if (c == QMetaObject::ReadProperty)
						{
							*value = model_.headerData( section_, orientation_, role );
						}
						else
						{
							model_.setHeaderData( section_, orientation_, *value, role );
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

		QAbstractItemModel & model_;
		int section_;
		Qt::Orientation orientation_;
		QList< int > roles_;
		std::unique_ptr< QMetaObject > metaObject_;
	};
}

struct WGItemView::Impl
{
	Impl() : model_( nullptr ) {}

	QAbstractItemModel * model_;
	QList< IModelExtension * > extensions_;
	QStringList roles_;
	std::unique_ptr< ExtendedModel > extendedModel_;
	std::unique_ptr< HeaderData > headerData_;
};

WGItemView::WGItemView()
	: impl_( new Impl() )
{
	impl_->extendedModel_.reset( new ExtendedModel( impl_->roles_, impl_->extensions_ ) );
}

WGItemView::~WGItemView()
{

}

QAbstractItemModel * WGItemView::getModel() const
{
	return impl_->model_;
}

void WGItemView::setModel( QAbstractItemModel * model )
{
	impl_->model_ = model;
	emit modelChanged();
	refresh();
}

QStringList WGItemView::getRoles() const
{
	return impl_->roles_;
}

void WGItemView::setRoles( const QStringList & roles )
{
	impl_->roles_ = roles;
	refresh();
}

QQmlListProperty< IModelExtension > WGItemView::getExtensions() const
{
	return QQmlListProperty< IModelExtension >(
		const_cast< WGItemView * >( this ),
		nullptr,
		&appendExtension, 
		&countExtensions,
		&extensionAt, 
		&clearExtensions );
}

void WGItemView::appendExtension( QQmlListProperty< IModelExtension > * property, IModelExtension * value )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return;
	}

	itemView->impl_->extensions_.append( value );
	itemView->refresh();
}

int WGItemView::countExtensions( QQmlListProperty< IModelExtension > * property )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return 0;
	}

	return itemView->impl_->extensions_.count();
}

IModelExtension * WGItemView::extensionAt( QQmlListProperty< IModelExtension > * property, int index )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return nullptr;
	}

	return itemView->impl_->extensions_[index];
}

void WGItemView::clearExtensions( QQmlListProperty< IModelExtension > * property )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return;
	}

	itemView->impl_->extensions_.clear();
	itemView->refresh();
}

QAbstractItemModel * WGItemView::getExtendedModel() const
{
	return impl_->extendedModel_.get();
}

QObject * WGItemView::getHeaderData() const
{
	return impl_->headerData_.get();
}

void WGItemView::refresh()
{
	impl_->extendedModel_->reset( impl_->model_ );

	//Enable for headers once body works.
	/*
	impl_->headerData_.reset();
	if (impl_->extendedModel_ != nullptr)
	{
		impl_->headerData_.reset( new HeaderData( *impl_->extendedModel_, 0, Qt::Horizontal ) );
	}
	emit headerDataChanged();*/
}