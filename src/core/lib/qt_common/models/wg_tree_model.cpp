#include "wg_tree_model.hpp"

#include "data_model/i_item.hpp"
#include "qt_common/helpers/qt_helpers.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "qt_common/models/extensions/i_model_extension.hpp"
#include "qt_common/qt_connection_holder.hpp"
#include "qt_common/qt_image_provider.hpp"
#include "qt_model_helpers.hpp"
#include "reflection/object_handle.hpp"

#include <QApplication>
#include <QThread>


class WGTreeModel::Impl
{
public:
	Impl();
	static QModelIndex calculateParentIndex( const WGTreeModel& self,
		const IItem* pItem,
		int column );

	IQtFramework* qtFramework_;
	ITreeModel* source_;
	QtModelHelpers::Extensions extensions_;
	QtConnectionHolder connections_;
};


WGTreeModel::Impl::Impl()
	: qtFramework_( nullptr )
	, source_( nullptr )
	, extensions_()
	, connections_()
{
}


QModelIndex WGTreeModel::Impl::calculateParentIndex( const WGTreeModel& self,
	const IItem* pParentItem,
	int column )
{
	if (pParentItem == nullptr)
	{
		return QModelIndex();
	}

	auto itemIndex = self.impl_->source_->index( pParentItem );
	const int row = static_cast< int >( itemIndex.first );
	return self.createIndex( row, column, const_cast< IItem * >( pParentItem ) );
}


WGTreeModel::WGTreeModel()
	: impl_( new Impl() )
{
	impl_->qtFramework_ = Context::queryInterface< IQtFramework >();

	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::itemDataAboutToBeChangedThread, 
		this, &WGTreeModel::beginChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::itemDataChangedThread, 
		this, &WGTreeModel::endChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsAboutToBeInsertedThread, 
		this, &WGTreeModel::beginInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsInsertedThread, 
		this, &WGTreeModel::endInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsAboutToBeRemovedThread, 
		this, &WGTreeModel::beginRemoveRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsRemovedThread, 
		this, &WGTreeModel::endRemoveRows,
		Qt::BlockingQueuedConnection );
}

WGTreeModel::~WGTreeModel()
{
}

void WGTreeModel::source( ITreeModel * source )
{
	beginResetModel();
	if (impl_->source_ != nullptr)
	{
		impl_->source_->onPreDataChanged().remove< WGTreeModel,
			&WGTreeModel::onPreDataChanged >( this );
		impl_->source_->onPostDataChanged().remove< WGTreeModel,
			&WGTreeModel::onPostDataChanged >( this );
		impl_->source_->onPreItemsInserted().remove< WGTreeModel,
			&WGTreeModel::onPreItemsInserted >( this );
		impl_->source_->onPostItemsInserted().remove< WGTreeModel,
			&WGTreeModel::onPostItemsInserted >( this );
		impl_->source_->onPreItemsRemoved().remove< WGTreeModel,
			&WGTreeModel::onPreItemsRemoved >( this );
		impl_->source_->onPostItemsRemoved().remove< WGTreeModel,
			&WGTreeModel::onPostItemsRemoved >( this );
	}
	impl_->source_ = source;
	emit sourceChanged();
	if (impl_->source_ != nullptr)
	{
		impl_->source_->onPreDataChanged().add< WGTreeModel,
			&WGTreeModel::onPreDataChanged >( this );
		impl_->source_->onPostDataChanged().add< WGTreeModel,
			&WGTreeModel::onPostDataChanged >( this );
		impl_->source_->onPreItemsInserted().add< WGTreeModel,
			&WGTreeModel::onPreItemsInserted >( this );
		impl_->source_->onPostItemsInserted().add< WGTreeModel,
			&WGTreeModel::onPostItemsInserted >( this );
		impl_->source_->onPreItemsRemoved().add< WGTreeModel,
			&WGTreeModel::onPreItemsRemoved >( this );
		impl_->source_->onPostItemsRemoved().add< WGTreeModel,
			&WGTreeModel::onPostItemsRemoved >( this );
	}
	endResetModel();
}

const ITreeModel * WGTreeModel::source() const
{
	return impl_->source_;
}

void WGTreeModel::registerExtension( IModelExtension * extension )
{
	beginResetModel();
	extension->init( this );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::itemDataAboutToBeChanged, 
		extension, &IModelExtension::onDataAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::itemDataChanged, 
		extension, &IModelExtension::onDataChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::layoutAboutToBeChanged, 
		extension, &IModelExtension::onLayoutAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::layoutChanged, 
		extension, &IModelExtension::onLayoutChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsAboutToBeInserted, 
		extension, &IModelExtension::onRowsAboutToBeInserted );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsInserted, 
		extension, &IModelExtension::onRowsInserted );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsAboutToBeRemoved, 
		extension, &IModelExtension::onRowsAboutToBeRemoved );
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::rowsRemoved, 
		extension, &IModelExtension::onRowsRemoved );
	impl_->extensions_.emplace_back( extension );
	endResetModel();
}

QHash< int, QByteArray > WGTreeModel::roleNames() const
{
	auto roleNames = QAbstractItemModel::roleNames();
	for (const auto& extension : impl_->extensions_)
	{
		roleNames.unite( extension->roleNames() );
	}
	return roleNames;
}

QModelIndex WGTreeModel::index(
	int row, int column, const QModelIndex &parent ) const
{
	if (impl_->source_ == nullptr || parent.column() > 0)
	{
		return QModelIndex();
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	auto item = impl_->source_->item( row, parentItem );
	if (item != nullptr && column < item->columnCount())
	{
		return createIndex( row, column, item );
	}

	return QModelIndex();
}

QModelIndex WGTreeModel::parent( const QModelIndex &child ) const
{
	if (impl_->source_ == nullptr)
	{
		return QModelIndex();
	}

	assert( child.isValid() );
	auto childItem = 
		reinterpret_cast< IItem * >( child.internalPointer() );
	auto itemIndex = impl_->source_->index( childItem );
	auto parentItem = itemIndex.second;

	if (parentItem != nullptr)
	{
		const int column = 0;
		return Impl::calculateParentIndex( *this, parentItem, column );
	}

	return QModelIndex();
}

int WGTreeModel::rowCount( const QModelIndex &parent ) const
{
	if (impl_->source_ == nullptr || parent.column() > 0)
	{
		return 0;
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	return (int)impl_->source_->size( parentItem );
}

int WGTreeModel::columnCount( const QModelIndex &parent ) const
{
	if (impl_->source_ == nullptr || parent.column() > 0)
	{
		return 0;
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	if (parentItem != nullptr)
	{
		return parentItem->columnCount();
	}

	return 0;
}

bool WGTreeModel::hasChildren( const QModelIndex &parent ) const
{
	if (impl_->source_ == nullptr || parent.column() > 0)
	{
		return false;
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	if (parentItem != nullptr)
	{
		return !(impl_->source_->empty( parentItem ));
	}

	return false;
}

QVariant WGTreeModel::data( const QModelIndex &index, int role ) const
{
	if (impl_->source_ == nullptr || !index.isValid())
	{
		return QVariant( QVariant::Invalid );
	}

	auto item = reinterpret_cast< IItem * >( index.internalPointer() );
	if (item == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	switch (role)
	{
	case Qt::DisplayRole:
		return QString( item->getDisplayText( index.column() ) );

	case Qt::DecorationRole:
		auto thumbnail = item->getThumbnail( index.column() );
		if (thumbnail != nullptr)
		{
			auto qtImageProvider = dynamic_cast< QtImageProvider * >(
				impl_->qtFramework_->qmlEngine()->imageProvider( QtImageProvider::providerId() ) );
			if (qtImageProvider != nullptr)
			{
				auto imagePath = qtImageProvider->encodeImage( thumbnail );
				return qtImageProvider->requestImage( imagePath, nullptr, QSize() );
			}
		}
	}

	if (role < Qt::UserRole)
	{
		return QVariant( QVariant::Invalid );
	}

	for (auto &extension : impl_->extensions_)
	{
		auto data = extension->data( index, role );
		if (data.isValid())
		{
			return data;
		}
	}

	return QVariant( QVariant::Invalid );
}

bool WGTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (impl_->source_ == nullptr)
	{
		return false;
	}

	for (auto &extension : impl_->extensions_)
	{
		if (extension->setData( index, value, role ))
		{
			return true;
		}
	}

	return false;
}

QVariant WGTreeModel::getSource() const
{
	Variant variant = ObjectHandle( 
		const_cast< ITreeModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

void WGTreeModel::setSource( const QVariant & source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto treeModel = provider.getBase< ITreeModel >();
			if (treeModel != nullptr)
			{
				this->source( treeModel );
			}
		}
	}
}

QQmlListProperty< IModelExtension > WGTreeModel::getExtensions() const
{
	return QQmlListProperty< IModelExtension >(
		const_cast< WGTreeModel * >( this ),
		nullptr,
		&appendExtension, 
		&countExtensions,
		&extensionAt, 
		&clearExtensions );
}

void WGTreeModel::appendExtension( 
	QQmlListProperty< IModelExtension > * property, 
	IModelExtension * value )
{
	auto treeModel = qobject_cast< WGTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return;
	}

	treeModel->registerExtension( value );
}

IModelExtension * WGTreeModel::extensionAt( 
	QQmlListProperty< IModelExtension > * property, 
	int index )
{
	auto treeModel = qobject_cast< WGTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return nullptr;
	}

	return treeModel->impl_->extensions_[index];
}

void WGTreeModel::clearExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto treeModel = qobject_cast< WGTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return;
	}

	treeModel->beginResetModel();
	treeModel->impl_->connections_.reset();
	treeModel->impl_->extensions_.clear();
	treeModel->endResetModel();
}

int WGTreeModel::countExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto treeModel = qobject_cast< WGTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return 0;
	}

	return static_cast< int >( treeModel->impl_->extensions_.size() );
}


EVENT_IMPL1( WGTreeModel, ITreeModel, DataChanged, ChangeData )
EVENT_IMPL2( WGTreeModel, ITreeModel, ItemsInserted, InsertRows )
EVENT_IMPL2( WGTreeModel, ITreeModel, ItemsRemoved, RemoveRows )


EMIT_IMPL1( WGTreeModel, Data, Change, itemData, Changed )
EMIT_IMPL2( WGTreeModel, QAbstractItemModel, Insert, Rows, rows, Inserted )
EMIT_IMPL2( WGTreeModel, QAbstractItemModel, Remove, Rows, rows, Removed )
