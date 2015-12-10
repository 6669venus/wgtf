#include "wg_tree_list_adapter.hpp"
#include "qt_model_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/models/extensions/i_model_extension.hpp"

class WGTreeListAdapter::Impl
{
public:
	Impl();
	
	IQtFramework* qtFramework_;
	QModelIndex sourceIndex_; //gnelsontodo - should we use the persistent version here??
	QtModelHelpers::Extensions extensions_;
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

QAbstractItemModel * WGTreeListAdapter::model() const
{
	return const_cast< QAbstractItemModel * >( impl_->sourceIndex_.model() );
}

QModelIndex WGTreeListAdapter::adaptedIndex(int row, int column, const QModelIndex &parent) const
{
	auto m = model();

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

QHash< int, QByteArray > WGTreeListAdapter::roleNames() const
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

int WGTreeListAdapter::rowCount( const QModelIndex &parent ) const
{
	auto m = model();

	if (m == nullptr)
	{
		return 0;
	}

	return m->rowCount( impl_->sourceIndex_ );
}

QVariant WGTreeListAdapter::getSourceIndex() const
{
	return QVariant::fromValue( impl_->sourceIndex_ );
}

void WGTreeListAdapter::setSourceIndex( const QVariant & index )
{
	disconnect();

	QModelIndex idx = index.toModelIndex();
	impl_->sourceIndex_ = idx;

	connect();

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
