#include "wg_list_model.hpp"

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


class WGListModel::Impl
{
public:
	Impl();
	~Impl() {}
	static QModelIndex calculateParentIndex( const WGListModel& self,
		const IItem* pParentItem,
		int column );

	IQtFramework* qtFramework_;
	IListModel* model_;
	QVariant source_;
	QtModelHelpers::Extensions extensions_;
	QtConnectionHolder connections_;
	QHash< int, QByteArray > roleNames_;
};


WGListModel::Impl::Impl()
	: qtFramework_( nullptr )
	, model_( nullptr )
	, source_()
	, extensions_()
	, connections_()
{
}


QModelIndex WGListModel::Impl::calculateParentIndex( const WGListModel& self,
	const IItem* pParentItem,
	int column )
{
	return QModelIndex();
}


WGListModel::WGListModel()
	: QAbstractListModel()
	, impl_( new Impl() )
{
	impl_->qtFramework_ = Context::queryInterface< IQtFramework >();

	impl_->connections_ += QObject::connect( 
		this, &WGListModel::sourceChanged, 
		this, &WGListModel::onSourceChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::itemDataAboutToBeChangedThread, 
		this, &WGListModel::beginChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::itemDataChangedThread, 
		this, &WGListModel::endChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsAboutToBeInsertedThread, 
		this, &WGListModel::beginInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsInsertedThread, 
		this, &WGListModel::endInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsAboutToBeRemovedThread, 
		this, &WGListModel::beginRemoveRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsRemovedThread, 
		this, &WGListModel::endRemoveRows,
		Qt::BlockingQueuedConnection );
}

WGListModel::~WGListModel()
{
	setSource( QVariant() );
}


IListModel * WGListModel::getModel() const
{
	return impl_->model_;
}


bool WGListModel::canClear() const
{
	auto m = getModel();

	if (m == nullptr)
	{
		return false;
	}

	return m->canClear();
}


void WGListModel::clear()
{
	auto m = getModel();

	if (m == nullptr)
	{
		return;
	}
		
	m->clear();
}


QModelIndex WGListModel::index(
	int row, int column, const QModelIndex &parent ) const
{
	IListModel* model = getModel();
	if (model == nullptr)
	{
		return QModelIndex();
	}

	auto item = model->item( row );
	if (item != nullptr && column < item->columnCount())
	{
		return createIndex( row, column, item );
	}

	return QModelIndex();
}


int WGListModel::indexRow( const QModelIndex& index ) const
{
	return index.row();
}


QModelIndex WGListModel::find( QVariant data, QString roleName ) const
{
	for (auto row = 0; row < rowCount( QModelIndex() ); ++row )
	{
		auto index = this->index( row );
		if (data == this->data( index, roleName ))
		{
			return index;
		}
	}
	return QModelIndex();
}


QVariant WGListModel::data( const QModelIndex& index, QString roleName ) const
{
	auto role = impl_->roleNames_.key( roleName.toUtf8(), -1 );
	if (role < 0)
	{
		return QModelIndex();
	}

	return data( index, role );
}


void WGListModel::registerExtension( IModelExtension * extension )
{
	beginResetModel();
	extension->init( this, impl_->qtFramework_ );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::itemDataAboutToBeChanged, 
		extension, &IModelExtension::onDataAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::itemDataChanged, 
		extension, &IModelExtension::onDataChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::layoutAboutToBeChanged, 
		extension, &IModelExtension::onLayoutAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::layoutChanged, 
		extension, &IModelExtension::onLayoutChanged );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsAboutToBeInserted, 
		extension, &IModelExtension::onRowsAboutToBeInserted );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsInserted, 
		extension, &IModelExtension::onRowsInserted );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsAboutToBeRemoved, 
		extension, &IModelExtension::onRowsAboutToBeRemoved );
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::rowsRemoved, 
		extension, &IModelExtension::onRowsRemoved );
	impl_->extensions_.emplace_back( extension );
	endResetModel();
}

QHash< int, QByteArray > WGListModel::roleNames() const
{
	impl_->roleNames_ = QAbstractListModel::roleNames();

	for (const auto& extension : impl_->extensions_)
	{
		QHashIterator<int, QByteArray> itr( extension->roleNames() );

		while (itr.hasNext())
		{
			itr.next();
			impl_->roleNames_.insert( itr.key(), itr.value() );
		}
	}
	return impl_->roleNames_;
}

int WGListModel::rowCount( const QModelIndex &parent ) const
{
	// Qt Lists never have a valid parent
	// because it can't have child items
	IListModel* model = getModel();
	if (model == nullptr || parent.isValid())
	{
		return 0;
	}

	return (int)model->size();
}

int WGListModel::columnCount( const QModelIndex &parent ) const
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

QVariant WGListModel::data( const QModelIndex &index, int role ) const
{
	if (getModel() == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	assert( index.isValid() );
	auto item = reinterpret_cast< IItem * >( index.internalPointer() );

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

	for (const auto& extension : impl_->extensions_)
	{
		auto data = extension->data( index, role );
		if (data.isValid())
		{
			return data;
		}
	}

	return QVariant( QVariant::Invalid );
}

bool WGListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (getModel() == nullptr)
	{
		return false;
	}

	for (auto& extension : impl_->extensions_)
	{
		if (extension->setData( index, value, role ))
		{
			return true;
		}
	}

	return false;
}

const QVariant & WGListModel::getSource() const
{
	return impl_->source_;
}

void WGListModel::setSource( const QVariant & source )
{
	beginResetModel();
	IListModel* model = getModel();
	if (model != nullptr)
	{
		model->onPreDataChanged().remove< WGListModel,
			&WGListModel::onPreDataChanged >( this );
		model->onPostDataChanged().remove< WGListModel,
			&WGListModel::onPostDataChanged >( this );
		model->onPreItemsInserted().remove< WGListModel,
			&WGListModel::onPreItemsInserted >( this );
		model->onPostItemsInserted().remove< WGListModel,
			&WGListModel::onPostItemsInserted >( this );
		model->onPreItemsRemoved().remove< WGListModel,
			&WGListModel::onPreItemsRemoved >( this );
		model->onPostItemsRemoved().remove< WGListModel,
			&WGListModel::onPostItemsRemoved >( this );
	}
	impl_->source_ = source;
	emit sourceChanged();
	model = getModel();
	if (model != nullptr)
	{
		model->onPreDataChanged().add< WGListModel,
			&WGListModel::onPreDataChanged >( this );
		model->onPostDataChanged().add< WGListModel,
			&WGListModel::onPostDataChanged >( this );
		model->onPreItemsInserted().add< WGListModel,
			&WGListModel::onPreItemsInserted >( this );
		model->onPostItemsInserted().add< WGListModel,
			&WGListModel::onPostItemsInserted >( this );
		model->onPreItemsRemoved().add< WGListModel,
			&WGListModel::onPreItemsRemoved >( this );
		model->onPostItemsRemoved().add< WGListModel,
			&WGListModel::onPostItemsRemoved >( this );
	}
	endResetModel();
}


void WGListModel::onSourceChanged()
{
	IListModel * source = nullptr;
	
	Variant variant = QtHelpers::toVariant( getSource() );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			source = provider.getBase< IListModel >();
		}
	}

	impl_->model_ = source;
}


QQmlListProperty< IModelExtension > WGListModel::getExtensions() const
{
	return QQmlListProperty< IModelExtension >(
		const_cast< WGListModel * >( this ),
		nullptr,
		&appendExtension, 
		&countExtensions,
		&extensionAt, 
		&clearExtensions );
}

void WGListModel::appendExtension( 
	QQmlListProperty< IModelExtension > * property, 
	IModelExtension * value )
{
	auto listModel = qobject_cast< WGListModel * >( property->object );
	if (listModel == nullptr)
	{
		return;
	}

	listModel->registerExtension( value );
}

IModelExtension * WGListModel::extensionAt( 
	QQmlListProperty< IModelExtension > * property, 
	int index )
{
	auto listModel = qobject_cast< WGListModel * >( property->object );
	if (listModel == nullptr)
	{
		return nullptr;
	}

	return listModel->impl_->extensions_[index];
}

void WGListModel::clearExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto listModel = qobject_cast< WGListModel * >( property->object );
	if (listModel == nullptr)
	{
		return;
	}

	listModel->beginResetModel();
	listModel->impl_->connections_.reset();
	listModel->impl_->extensions_.clear();
	listModel->endResetModel();
}

int WGListModel::countExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto listModel = qobject_cast< WGListModel * >( property->object );
	if (listModel == nullptr)
	{
		return 0;
	}

	return static_cast< int >( listModel->impl_->extensions_.size() );
}

EVENT_IMPL1( WGListModel, IListModel, DataChanged, ChangeData )
EVENT_IMPL2( WGListModel, IListModel, ItemsInserted, InsertRows )
EVENT_IMPL2( WGListModel, IListModel, ItemsRemoved, RemoveRows )

EMIT_IMPL1( WGListModel, Data, Change, itemData, Changed )
EMIT_IMPL2( WGListModel, QAbstractListModel, Insert, Rows, rows, Inserted )
EMIT_IMPL2( WGListModel, QAbstractListModel, Remove, Rows, rows, Removed )

