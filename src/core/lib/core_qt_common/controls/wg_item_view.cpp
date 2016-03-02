#include "wg_item_view.hpp"
#include "models/extensions/i_model_extension.hpp"

#include <QAbstractItemModel>
#include <QQmlListProperty>
#include <QString>
#include <QUuid>

#include <private/qmetaobjectbuilder_p.h>

namespace
{
	class ExtendedModel : public QAbstractItemModel
	{
	public:
		ExtendedModel( QAbstractItemModel & model, QList< IModelExtension * > & extensions )
			: model_( model )
			, extensions_( extensions )
		{
			roleNames_ = model_.roleNames();
			for (auto & extension : extensions_)
			{
				QHashIterator<int, QByteArray> itr( extension->roleNames() );
				while (itr.hasNext())
				{
					itr.next();
					roleNames_.insert( itr.key(), itr.value() );
				}
			}
		}

	private:
		QModelIndex index( int row, int column, const QModelIndex &parent ) const override
		{
			auto index = model_.index( row, column, parent );
			return createIndex( index.row(), index.column(), index.internalId() );
		}

		QModelIndex parent( const QModelIndex &child) const override
		{
			auto parent = model_.parent( child );
			return createIndex( parent.row(), parent.column(), parent.internalId() );
		}

		int rowCount( const QModelIndex &parent ) const override
		{
			return model_.rowCount( parent );
		}

		int columnCount( const QModelIndex &parent ) const override
		{
			return model_.columnCount( parent );
		}

		bool hasChildren( const QModelIndex &parent ) const override
		{
			return model_.hasChildren( parent );
		}

		QVariant data( const QModelIndex &index, int role ) const override
		{
			for (auto & extension : extensions_)
			{
				auto data = extension->data( index, role );
				if (data.isValid())
				{
					return data;
				}
			}

			return model_.data( index, role );
		}

		bool setData( const QModelIndex &index, const QVariant &value, int role ) override
		{
			for (auto & extension : extensions_)
			{
				if (extension->setData( index, value, role ) )
				{
					return true;
				}
			}

			return model_.setData( index, value, role );
		}

		QVariant headerData( int section, Qt::Orientation orientation, int role ) const override
		{ 
			for (auto & extension : extensions_)
			{
				auto data = extension->headerData( section, orientation, role );
				if (data.isValid())
				{
					return data;
				}
			}

			return model_.headerData( section, orientation, role );
		}

		bool setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role ) override
		{
			for (auto & extension : extensions_)
			{
				if (extension->setHeaderData( section, orientation, value, role ) )
				{
					return true;
				}
			}

			return model_.setHeaderData( section, orientation, value, role );
		}

		QHash< int, QByteArray > roleNames() const override
		{
			return roleNames_;
		}

		QAbstractItemModel & model_;
		QList< IModelExtension * > & extensions_;
		QHash< int, QByteArray > roleNames_;
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
	QAbstractItemModel * model_;
	QList< IModelExtension * > extensions_;
	std::unique_ptr< ExtendedModel > extendedModel_;
	std::unique_ptr< HeaderData > headerData_;
};

WGItemView::WGItemView()
	: impl_( new Impl() )
{

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
	impl_->extendedModel_.reset();
	if (impl_->model_ != nullptr)
	{
		impl_->extendedModel_.reset( new ExtendedModel( *impl_->model_, impl_->extensions_ ) );
	}
	emit extendedModelChanged();

	impl_->headerData_.reset();
	if (impl_->extendedModel_ != nullptr)
	{
		impl_->headerData_.reset( new HeaderData( *impl_->extendedModel_, 0, Qt::Horizontal ) );
	}
	emit headerDataChanged();
}