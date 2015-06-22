#include "qt_list_model.hpp"

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


class QtListModel::Impl
{
public:
	Impl();
	static QModelIndex calculateParentIndex( const QtListModel& self,
		const IItem* pParentItem,
		int column );

	IQtFramework* qtFramework_;
	IListModel* source_;
	QtModelHelpers::Extensions extensions_;
	QtConnectionHolder connections_;
};


QtListModel::Impl::Impl()
	: qtFramework_( nullptr )
	, source_( nullptr )
	, extensions_()
	, connections_()
{
}


QModelIndex QtListModel::Impl::calculateParentIndex( const QtListModel& self,
	const IItem* pParentItem,
	int column )
{
	return QModelIndex();
}


QtListModel::QtListModel()
	: QAbstractListModel()
	, impl_( new Impl() )
{
	impl_->qtFramework_ = Context::queryInterface< IQtFramework >();

	impl_->connections_ += QObject::connect( 
		this, &QtListModel::itemDataAboutToBeChangedThread, 
		this, &QtListModel::beginChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::itemDataChangedThread, 
		this, &QtListModel::endChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsAboutToBeInsertedThread, 
		this, &QtListModel::beginInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsInsertedThread, 
		this, &QtListModel::endInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsAboutToBeRemovedThread, 
		this, &QtListModel::beginRemoveRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsRemovedThread, 
		this, &QtListModel::endRemoveRows,
		Qt::BlockingQueuedConnection );
}

QtListModel::~QtListModel()
{
}

void QtListModel::source( IListModel * source )
{
	beginResetModel();
	if (impl_->source_ != nullptr)
	{
		impl_->source_->onPreDataChanged().remove< QtListModel,
			&QtListModel::onPreDataChanged >( this );
		impl_->source_->onPostDataChanged().remove< QtListModel,
			&QtListModel::onPostDataChanged >( this );
		impl_->source_->onPreItemsInserted().remove< QtListModel,
			&QtListModel::onPreItemsInserted >( this );
		impl_->source_->onPostItemsInserted().remove< QtListModel,
			&QtListModel::onPostItemsInserted >( this );
		impl_->source_->onPreItemsRemoved().remove< QtListModel,
			&QtListModel::onPreItemsRemoved >( this );
		impl_->source_->onPostItemsRemoved().remove< QtListModel,
			&QtListModel::onPostItemsRemoved >( this );
	}
	impl_->source_ = source;
	emit sourceChanged();
	if (impl_->source_ != nullptr)
	{
		impl_->source_->onPreDataChanged().add< QtListModel,
			&QtListModel::onPreDataChanged >( this );
		impl_->source_->onPostDataChanged().add< QtListModel,
			&QtListModel::onPostDataChanged >( this );
		impl_->source_->onPreItemsInserted().add< QtListModel,
			&QtListModel::onPreItemsInserted >( this );
		impl_->source_->onPostItemsInserted().add< QtListModel,
			&QtListModel::onPostItemsInserted >( this );
		impl_->source_->onPreItemsRemoved().add< QtListModel,
			&QtListModel::onPreItemsRemoved >( this );
		impl_->source_->onPostItemsRemoved().add< QtListModel,
			&QtListModel::onPostItemsRemoved >( this );
	}
	endResetModel();
}

const IListModel * QtListModel::source() const
{
	return impl_->source_;
}


bool QtListModel::canClear() const
{
	return impl_->source_->canClear();
}


void QtListModel::clear()
{
	impl_->source_->clear();
}


void QtListModel::registerExtension( IModelExtension * extension )
{
	beginResetModel();
	extension->init( this );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::itemDataAboutToBeChanged, 
		extension, &IModelExtension::onDataAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::itemDataChanged, 
		extension, &IModelExtension::onDataChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::layoutAboutToBeChanged, 
		extension, &IModelExtension::onLayoutAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::layoutChanged, 
		extension, &IModelExtension::onLayoutChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsAboutToBeInserted, 
		extension, &IModelExtension::onRowsAboutToBeInserted );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsInserted, 
		extension, &IModelExtension::onRowsInserted );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsAboutToBeRemoved, 
		extension, &IModelExtension::onRowsAboutToBeRemoved );
	impl_->connections_ += QObject::connect( 
		this, &QtListModel::rowsRemoved, 
		extension, &IModelExtension::onRowsRemoved );
	impl_->extensions_.emplace_back( extension );
	endResetModel();
}

QHash< int, QByteArray > QtListModel::roleNames() const
{
	auto roleNames = QAbstractListModel::roleNames();
	for (const auto& extension : impl_->extensions_)
	{
		roleNames.unite( extension->roleNames() );
	}
	return roleNames;
}

QModelIndex QtListModel::index(
	int row, int column, const QModelIndex &parent ) const
{
	if (impl_->source_ == nullptr)
	{
		return QModelIndex();
	}

	auto item = impl_->source_->item( row );
	if (item != nullptr && column < item->columnCount())
	{
		return createIndex( row, column, item );
	}

	return QModelIndex();
}

int QtListModel::rowCount( const QModelIndex &parent ) const
{
	// Qt Lists never have a valid parent
	// because it can't have child items
	if (impl_->source_ == nullptr || parent.isValid())
	{
		return 0;
	}

	return (int)impl_->source_->size();
}

int QtListModel::columnCount( const QModelIndex &parent ) const
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

QVariant QtListModel::data( const QModelIndex &index, int role ) const
{
	if (impl_->source_ == nullptr)
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

bool QtListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (impl_->source_ == nullptr)
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

QVariant QtListModel::getSource() const
{
	Variant variant = ObjectHandle( 
		const_cast< IListModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

void QtListModel::setSource( const QVariant & source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto listModel = provider.getBase< IListModel >();
			if (listModel != nullptr)
			{
				this->source( listModel );
			}
		}
	}
}


QQmlListProperty< IModelExtension > QtListModel::getExtensions() const
{
	return QQmlListProperty< IModelExtension >(
		const_cast< QtListModel * >( this ),
		nullptr,
		&appendExtension, 
		&countExtensions,
		&extensionAt, 
		&clearExtensions );
}

void QtListModel::appendExtension( 
	QQmlListProperty< IModelExtension > * property, 
	IModelExtension * value )
{
	auto listModel = qobject_cast< QtListModel * >( property->object );
	if (listModel == nullptr)
	{
		return;
	}

	listModel->registerExtension( value );
}

IModelExtension * QtListModel::extensionAt( 
	QQmlListProperty< IModelExtension > * property, 
	int index )
{
	auto listModel = qobject_cast< QtListModel * >( property->object );
	if (listModel == nullptr)
	{
		return nullptr;
	}

	return listModel->impl_->extensions_[index];
}

void QtListModel::clearExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto listModel = qobject_cast< QtListModel * >( property->object );
	if (listModel == nullptr)
	{
		return;
	}

	listModel->beginResetModel();
	listModel->impl_->connections_.reset();
	listModel->impl_->extensions_.clear();
	listModel->endResetModel();
}

int QtListModel::countExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto listModel = qobject_cast< QtListModel * >( property->object );
	if (listModel == nullptr)
	{
		return 0;
	}

	return static_cast< int >( listModel->impl_->extensions_.size() );
}

EVENT_IMPL1( QtListModel, IListModel, DataChanged, ChangeData )
EVENT_IMPL2( QtListModel, IListModel, ItemsInserted, InsertRows )
EVENT_IMPL2( QtListModel, IListModel, ItemsRemoved, RemoveRows )

EMIT_IMPL1( QtListModel, Data, Change, itemData, Changed )
EMIT_IMPL2( QtListModel, QAbstractListModel, Insert, Rows, rows, Inserted )
EMIT_IMPL2( QtListModel, QAbstractListModel, Remove, Rows, rows, Removed )

