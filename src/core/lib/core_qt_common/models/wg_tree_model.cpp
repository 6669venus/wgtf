#include "wg_tree_model.hpp"

#include "core_data_model/i_item.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/models/extensions/i_model_extension.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_qt_common/qt_image_provider.hpp"
#include "qt_model_helpers.hpp"
#include "core_reflection/object_handle.hpp"

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
	ITreeModel* model_;
	QVariant source_;
	QtModelHelpers::Extensions extensions_;
	QtConnectionHolder connections_;
};


WGTreeModel::Impl::Impl()
	: qtFramework_( nullptr )
	, model_( nullptr )
	, source_()
	, extensions_()
	, connections_()
{
}


QModelIndex WGTreeModel::Impl::calculateParentIndex( const WGTreeModel& self,
	const IItem* pParentItem,
	int column )
{
	ITreeModel* model = self.getModel();
	if (pParentItem == nullptr || model == nullptr)
	{
		return QModelIndex();
	}

	auto itemIndex = model->index( pParentItem );
	const int row = static_cast< int >( itemIndex.first );
	return self.createIndex( row, column, const_cast< IItem * >( pParentItem ) );
}


WGTreeModel::WGTreeModel()
	: impl_( new Impl() )
{
	impl_->qtFramework_ = Context::queryInterface< IQtFramework >();

	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::sourceChanged, 
		this, &WGTreeModel::onSourceChanged );
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
	std::string modelName = this->objectName().toUtf8().constData();
	for (const auto& extension : impl_->extensions_)
	{
		extension->saveStates( modelName.c_str() );
	}

	setSource( QVariant() );
}

ITreeModel * WGTreeModel::getModel() const
{
	return impl_->model_;
}

void WGTreeModel::registerExtension( IModelExtension * extension )
{
	beginResetModel();
	extension->init( this );
	std::string modelName = this->objectName().toUtf8().constData();
	extension->loadStates( modelName.c_str() );
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
		QHashIterator<int, QByteArray> itr( extension->roleNames() );

		while (itr.hasNext())
		{
			itr.next();
			roleNames.insert( itr.key(), itr.value() );
		}
	}

	return roleNames;
}

QModelIndex WGTreeModel::index(
	int row, int column, const QModelIndex &parent ) const
{
	ITreeModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return QModelIndex();
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	auto item = model->item( row, parentItem );
	if (item != nullptr && column < item->columnCount())
	{
		return createIndex( row, column, item );
	}

	return QModelIndex();
}

QModelIndex WGTreeModel::parent( const QModelIndex &child ) const
{
	ITreeModel* model = getModel();
	if (model == nullptr)
	{
		return QModelIndex();
	}

	assert( child.isValid() );
	auto childItem = 
		reinterpret_cast< IItem * >( child.internalPointer() );
	auto itemIndex = model->index( childItem );
	auto parentItem = itemIndex.second;

	if (parentItem != nullptr)
	{
		const int column = 0;
		return Impl::calculateParentIndex( *this, parentItem, column );
	}

	return QModelIndex();
}

QModelIndex WGTreeModel::convertItemIndex( const QVariant & treeIndex ) const
{
	auto variant = QtHelpers::toVariant( treeIndex );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto itemIndex = provider.getBase< ITreeModel::ItemIndex >();
			if (itemIndex != nullptr)
			{
				const int row = static_cast< int >( itemIndex->first );
				auto item = impl_->model_->item( row, itemIndex->second );
				if (item != nullptr)
				{
					return createIndex( row, 0, item );
				}
			}
		}
	}
	
	return QModelIndex();
}

int WGTreeModel::rowCount( const QModelIndex &parent ) const
{
	ITreeModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return 0;
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	return (int)model->size( parentItem );
}

int WGTreeModel::columnCount( const QModelIndex &parent ) const
{
	if (getModel() == nullptr || parent.column() > 0)
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
	ITreeModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return false;
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	if (parentItem != nullptr)
	{
		return !(model->empty( parentItem ));
	}

	return false;
}

QVariant WGTreeModel::data( const QModelIndex &index, int role ) const
{
	if (getModel() == nullptr || !index.isValid())
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
	if (getModel() == nullptr)
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

const QVariant & WGTreeModel::getSource() const
{
	return impl_->source_;
}

void WGTreeModel::setSource( const QVariant & source )
{
	beginResetModel();
	ITreeModel* model = getModel();
	if (model != nullptr)
	{
		model->onPreDataChanged().remove< WGTreeModel, &WGTreeModel::onPreDataChanged >( this );
		model->onPostDataChanged().remove< WGTreeModel, &WGTreeModel::onPostDataChanged >( this );
		model->onPreItemsInserted().remove< WGTreeModel, &WGTreeModel::onPreItemsInserted >( this );
		model->onPostItemsInserted().remove< WGTreeModel, &WGTreeModel::onPostItemsInserted >( this );
		model->onPreItemsRemoved().remove< WGTreeModel, &WGTreeModel::onPreItemsRemoved >( this );
		model->onPostItemsRemoved().remove< WGTreeModel, &WGTreeModel::onPostItemsRemoved >( this );
	}
	impl_->source_ = source;
	emit sourceChanged();
	model = getModel();
	if (model != nullptr)
	{
		model->onPreDataChanged().add< WGTreeModel, &WGTreeModel::onPreDataChanged >( this );
		model->onPostDataChanged().add< WGTreeModel, &WGTreeModel::onPostDataChanged >( this );
		model->onPreItemsInserted().add< WGTreeModel, &WGTreeModel::onPreItemsInserted >( this );
		model->onPostItemsInserted().add< WGTreeModel, &WGTreeModel::onPostItemsInserted >( this );
		model->onPreItemsRemoved().add< WGTreeModel, &WGTreeModel::onPreItemsRemoved >( this );
		model->onPostItemsRemoved().add< WGTreeModel, &WGTreeModel::onPostItemsRemoved >( this );
	}
	endResetModel();
}


void WGTreeModel::onSourceChanged()
{
	ITreeModel * source = nullptr;

	Variant variant = QtHelpers::toVariant( getSource() );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			source = provider.getBase< ITreeModel >();
		}
	}

	impl_->model_ = source;
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
