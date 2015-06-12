#include "qt_tree_model.hpp"

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


class QtTreeModel::Impl
{
public:
	Impl();
	static QModelIndex calculateParentIndex( const QtTreeModel& self,
		const IItem* pItem,
		int column );

	IQtFramework* qtFramework_;
	ITreeModel* source_;
	QtModelHelpers::Extensions extensions_;
	QtConnectionHolder connections_;
};


QtTreeModel::Impl::Impl()
	: qtFramework_( nullptr )
	, source_( nullptr )
	, extensions_()
	, connections_()
{
}


QModelIndex QtTreeModel::Impl::calculateParentIndex( const QtTreeModel& self,
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


QtTreeModel::QtTreeModel()
	: impl_( new Impl() )
{
	impl_->qtFramework_ = Context::queryInterface< IQtFramework >();

	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::itemDataAboutToBeChangedThread, 
		this, &QtTreeModel::beginChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::itemDataChangedThread, 
		this, &QtTreeModel::endChangeData,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsAboutToBeInsertedThread, 
		this, &QtTreeModel::beginInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsInsertedThread, 
		this, &QtTreeModel::endInsertRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsAboutToBeRemovedThread, 
		this, &QtTreeModel::beginRemoveRows,
		Qt::BlockingQueuedConnection );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsRemovedThread, 
		this, &QtTreeModel::endRemoveRows,
		Qt::BlockingQueuedConnection );
}

QtTreeModel::~QtTreeModel()
{
}

void QtTreeModel::source( ITreeModel * source )
{
	beginResetModel();
	if (impl_->source_ != nullptr)
	{
		impl_->source_->onPreDataChanged().remove< QtTreeModel,
			&QtTreeModel::onPreDataChanged >( this );
		impl_->source_->onPostDataChanged().remove< QtTreeModel,
			&QtTreeModel::onPostDataChanged >( this );
		impl_->source_->onPreItemsInserted().remove< QtTreeModel,
			&QtTreeModel::onPreItemsInserted >( this );
		impl_->source_->onPostItemsInserted().remove< QtTreeModel,
			&QtTreeModel::onPostItemsInserted >( this );
		impl_->source_->onPreItemsRemoved().remove< QtTreeModel,
			&QtTreeModel::onPreItemsRemoved >( this );
		impl_->source_->onPostItemsRemoved().remove< QtTreeModel,
			&QtTreeModel::onPostItemsRemoved >( this );
	}
	impl_->source_ = source;
	emit sourceChanged();
	if (impl_->source_ != nullptr)
	{
		impl_->source_->onPreDataChanged().add< QtTreeModel,
			&QtTreeModel::onPreDataChanged >( this );
		impl_->source_->onPostDataChanged().add< QtTreeModel,
			&QtTreeModel::onPostDataChanged >( this );
		impl_->source_->onPreItemsInserted().add< QtTreeModel,
			&QtTreeModel::onPreItemsInserted >( this );
		impl_->source_->onPostItemsInserted().add< QtTreeModel,
			&QtTreeModel::onPostItemsInserted >( this );
		impl_->source_->onPreItemsRemoved().add< QtTreeModel,
			&QtTreeModel::onPreItemsRemoved >( this );
		impl_->source_->onPostItemsRemoved().add< QtTreeModel,
			&QtTreeModel::onPostItemsRemoved >( this );
	}
	endResetModel();
}

const ITreeModel * QtTreeModel::source() const
{
	return impl_->source_;
}

void QtTreeModel::registerExtension( IModelExtension * extension )
{
	beginResetModel();
	extension->init( this );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::itemDataAboutToBeChanged, 
		extension, &IModelExtension::onDataAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::itemDataChanged, 
		extension, &IModelExtension::onDataChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::layoutAboutToBeChanged, 
		extension, &IModelExtension::onLayoutAboutToBeChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::layoutChanged, 
		extension, &IModelExtension::onLayoutChanged );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsAboutToBeInserted, 
		extension, &IModelExtension::onRowsAboutToBeInserted );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsInserted, 
		extension, &IModelExtension::onRowsInserted );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsAboutToBeRemoved, 
		extension, &IModelExtension::onRowsAboutToBeRemoved );
	impl_->connections_ += QObject::connect( 
		this, &QtTreeModel::rowsRemoved, 
		extension, &IModelExtension::onRowsRemoved );
	impl_->extensions_.emplace_back( extension );
	endResetModel();
}

QHash< int, QByteArray > QtTreeModel::roleNames() const
{
	auto roleNames = QAbstractItemModel::roleNames();
	for (const auto& extension : impl_->extensions_)
	{
		roleNames.unite( extension->roleNames() );
	}
	return roleNames;
}

QModelIndex QtTreeModel::index(
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

QModelIndex QtTreeModel::parent( const QModelIndex &child ) const
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

int QtTreeModel::rowCount( const QModelIndex &parent ) const
{
	if (impl_->source_ == nullptr || parent.column() > 0)
	{
		return 0;
	}

	auto parentItem = !parent.isValid() ? nullptr :
		reinterpret_cast< IItem * >( parent.internalPointer() );
	return (int)impl_->source_->size( parentItem );
}

int QtTreeModel::columnCount( const QModelIndex &parent ) const
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

bool QtTreeModel::hasChildren( const QModelIndex &parent ) const
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

QVariant QtTreeModel::data( const QModelIndex &index, int role ) const
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

bool QtTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

QVariant QtTreeModel::getSource() const
{
	Variant variant = ObjectHandle( 
		const_cast< ITreeModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

void QtTreeModel::setSource( const QVariant & source )
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

QQmlListProperty< IModelExtension > QtTreeModel::getExtensions() const
{
	return QQmlListProperty< IModelExtension >(
		const_cast< QtTreeModel * >( this ),
		nullptr,
		&appendExtension, 
		&countExtensions,
		&extensionAt, 
		&clearExtensions );
}

void QtTreeModel::appendExtension( 
	QQmlListProperty< IModelExtension > * property, 
	IModelExtension * value )
{
	auto treeModel = qobject_cast< QtTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return;
	}

	treeModel->registerExtension( value );
}

IModelExtension * QtTreeModel::extensionAt( 
	QQmlListProperty< IModelExtension > * property, 
	int index )
{
	auto treeModel = qobject_cast< QtTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return nullptr;
	}

	return treeModel->impl_->extensions_[index];
}

void QtTreeModel::clearExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto treeModel = qobject_cast< QtTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return;
	}

	treeModel->beginResetModel();
	treeModel->impl_->connections_.reset();
	treeModel->impl_->extensions_.clear();
	treeModel->endResetModel();
}

int QtTreeModel::countExtensions( 
	QQmlListProperty< IModelExtension > * property )
{
	auto treeModel = qobject_cast< QtTreeModel * >( property->object );
	if (treeModel == nullptr)
	{
		return 0;
	}

	return static_cast< int >( treeModel->impl_->extensions_.size() );
}


EVENT_IMPL1( QtTreeModel, ITreeModel, DataChanged, ChangeData )
EVENT_IMPL2( QtTreeModel, ITreeModel, ItemsInserted, InsertRows )
EVENT_IMPL2( QtTreeModel, ITreeModel, ItemsRemoved, RemoveRows )


EMIT_IMPL1( QtTreeModel, Data, Change, itemData, Changed )
EMIT_IMPL2( QtTreeModel, QAbstractItemModel, Insert, Rows, rows, Inserted )
EMIT_IMPL2( QtTreeModel, QAbstractItemModel, Remove, Rows, rows, Removed )
