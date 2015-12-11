#include "wg_tree_list_adapter.hpp"
#include "qt_model_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/models/extensions/i_model_extension.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"

class WGTreeListAdapter::Impl
{
public:
	Impl();
	
	IQtFramework* qtFramework_;
	QModelIndex sourceIndex_; //gnelsontodo - should we use the persistent version here like ChildListAdapter??
	QModelIndex removedParent_;
	QtModelHelpers::Extensions extensions_;
	QHash<int, QByteArray> roleNames_;
};

WGTreeListAdapter::Impl::Impl()
	: qtFramework_( nullptr )
	, extensions_()
{
}

WGTreeListAdapter::WGTreeListAdapter()
	: impl_( new Impl() )
{
	impl_->qtFramework_ = Context::queryInterface< IQtFramework >();
}

WGTreeListAdapter::~WGTreeListAdapter()
{
	std::string modelName = this->objectName().toUtf8().constData();
	for (const auto& extension : impl_->extensions_)
	{
		extension->saveStates( modelName.c_str() );
	}

	disconnect();
}

QAbstractItemModel * WGTreeListAdapter::sourceModel() const
{
	return const_cast< QAbstractItemModel * >( impl_->sourceIndex_.model() );
}

QAbstractItemModel * WGTreeListAdapter::model() const
{
	auto model = static_cast<const QAbstractItemModel*>( this );
	return const_cast<QAbstractItemModel*>( model );
}

QHash< int, QByteArray > WGTreeListAdapter::roleNames() const
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

QModelIndex WGTreeListAdapter::adaptedIndex(int row, int column, const QModelIndex &parent) const
{
	auto m = sourceModel();

	if (m == nullptr)
	{
		return QModelIndex();
	}

	return m->index( row, column, impl_->sourceIndex_ );
}

void WGTreeListAdapter::registerExtension( IModelExtension * extension )
{
	beginResetModel();
	extension->init( this, impl_->qtFramework_ );
	std::string modelName = this->objectName().toUtf8().constData();
	extension->loadStates( modelName.c_str() );
	impl_->extensions_.emplace_back( extension );
	endResetModel();
}

int WGTreeListAdapter::rowCount( const QModelIndex &parent ) const
{
	auto m = sourceModel();

	if (m == nullptr)
	{
		return 0;
	}

	return m->rowCount( impl_->sourceIndex_ );
}

int WGTreeListAdapter::columnCount( const QModelIndex &parent ) const
{
	auto m = sourceModel();

	if (m == nullptr)
	{
		return 0;
	}

	return m->columnCount( impl_->sourceIndex_ );
}

QVariant WGTreeListAdapter::data(const QModelIndex &index, int role) const
{
	if (sourceModel() == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	assert( index.isValid() );

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

bool WGTreeListAdapter::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (sourceModel() == nullptr)
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

QVariant WGTreeListAdapter::getSourceIndex() const
{
	return QVariant::fromValue( impl_->sourceIndex_ );
}

void WGTreeListAdapter::setSourceIndex( const QVariant & index )
{
	beginResetModel();
	disconnect();

	QModelIndex idx = index.toModelIndex();
	impl_->sourceIndex_ = idx;
	reset();

	connect();
	endResetModel();
	
	emit sourceIndexChanged();
}

QQmlListProperty< IModelExtension > WGTreeListAdapter::getExtensions() const
{
	return QQmlListProperty< IModelExtension >(
		const_cast< WGTreeListAdapter * >( this ),
		nullptr,
		&appendExtension, 
		&countExtensions,
		&extensionAt, 
		&clearExtensions );
}

void WGTreeListAdapter::appendExtension( QQmlListProperty< IModelExtension > * property, IModelExtension * value )
{
	auto treeListAdapter = qobject_cast< WGTreeListAdapter * >( property->object );
	if (treeListAdapter == nullptr)
	{
		return;
	}

	treeListAdapter->registerExtension( value );
}

IModelExtension * WGTreeListAdapter::extensionAt( QQmlListProperty< IModelExtension > * property, int index )
{
	auto treeListAdapter = qobject_cast< WGTreeListAdapter * >( property->object );
	if (treeListAdapter == nullptr)
	{
		return nullptr;
	}

	return treeListAdapter->impl_->extensions_[index];
}

void WGTreeListAdapter::clearExtensions( QQmlListProperty< IModelExtension > * property )
{
	auto treeListAdapter = qobject_cast< WGTreeListAdapter * >( property->object );
	if (treeListAdapter == nullptr)
	{
		return;
	}

	treeListAdapter->beginResetModel();
	treeListAdapter->impl_->extensions_.clear();
	treeListAdapter->endResetModel();
}

int WGTreeListAdapter::countExtensions( QQmlListProperty< IModelExtension > * property )
{
	auto treeListAdapter = qobject_cast< WGTreeListAdapter * >( property->object );
	if (treeListAdapter == nullptr)
	{
		return 0;
	}

	return static_cast< int >( treeListAdapter->impl_->extensions_.size() );
}

void WGTreeListAdapter::onParentDataChanged(const QModelIndex &topLeft, 
	const QModelIndex &bottomRight, const QVector<int> &roles)
{
	if (topLeft.parent() == impl_->sourceIndex_ && bottomRight.parent() == impl_->sourceIndex_)
	{
		emit dataChanged( 
			createIndex(topLeft.row(), topLeft.column()), 
			createIndex(bottomRight.row(), bottomRight.column()), 
			roles );
	}
}

void WGTreeListAdapter::onParentLayoutAboutToBeChanged(const QList<QPersistentModelIndex> & parents, 
												 QAbstractItemModel::LayoutChangeHint hint)
{
	auto resetLayout = parents.empty();
	if (!resetLayout)
	{
		auto item = static_cast< QModelIndex >( impl_->sourceIndex_ ).internalPointer();
		for (auto it = parents.cbegin(); it != parents.cend(); ++it)
		{
			if (static_cast< QModelIndex >( *it ).internalPointer() == item)
			{
				resetLayout = true;
				break;
			}
		}
	}

	if (resetLayout)
	{
		reset();
		emit layoutAboutToBeChanged();
	}
}

void WGTreeListAdapter::onParentLayoutChanged(const QList<QPersistentModelIndex> & parents, 
										QAbstractItemModel::LayoutChangeHint hint)
{
	auto resetLayout = parents.empty();
	if (!resetLayout)
	{
		auto item = static_cast< QModelIndex >( impl_->sourceIndex_ ).internalPointer();
		for (auto it = parents.cbegin(); it != parents.cend(); ++it)
		{
			if (static_cast< QModelIndex >( *it ).internalPointer() == item)
			{
				resetLayout = true;
				break;
			}
		}
	}

	if (resetLayout)
	{
		emit layoutChanged();
	}
}

void WGTreeListAdapter::onParentRowsAboutToBeInserted(const QModelIndex & parent, int first, int last)
{
	if (parent != impl_->sourceIndex_)
	{
		return;
	}

	beginInsertRows( QModelIndex(), first, last );
}

void WGTreeListAdapter::onParentRowsInserted(const QModelIndex & parent, int first, int last)
{
	if (parent != impl_->sourceIndex_)
	{
		return;
	}

	reset();
	endInsertRows();
}

void WGTreeListAdapter::onParentRowsAboutToBeRemoved(const QModelIndex & parent, int first, int last)
{
	if (parent != impl_->sourceIndex_)
	{
		return;
	}

	impl_->removedParent_ = impl_->sourceIndex_;
	beginRemoveRows( QModelIndex(), first, last );
}

void WGTreeListAdapter::onParentRowsRemoved(const QModelIndex & parent, int first, int last)
{
	if (impl_->removedParent_ == QModelIndex())
	{
		return;
	}

	impl_->removedParent_ = QModelIndex();
	reset();
	endRemoveRows();
}
