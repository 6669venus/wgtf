#include "tree_extension.hpp"
#include "core_qt_common/models/adapters/child_list_adapter.hpp"
#include "core_qt_common/models/adapters/indexed_adapter.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/i_qt_framework.hpp"

ITEMROLE( childModel )
ITEMROLE( hasChildren )
ITEMROLE( expanded )

struct TreeExtension::Implementation
{
	Implementation( TreeExtension& self );
	~Implementation();
	void expand( const QModelIndex& index );
	void collapse( const QModelIndex& index );
	bool expanded( const QModelIndex& index ) const;

	TreeExtension& self_;
	std::vector< IndexedAdapter< ChildListAdapter > > childModels_;
	std::vector< std::unique_ptr< ChildListAdapter > > redundantChildModels_;
	std::vector< QPersistentModelIndex > expanded_;
};

TreeExtension::Implementation::Implementation( TreeExtension & self )
	: self_( self )
{

}

TreeExtension::Implementation::~Implementation()
{
}

void TreeExtension::Implementation::expand( const QModelIndex& index )
{
	if (!expanded( index ))
	{
		expanded_.push_back( index );
	}
}


void TreeExtension::Implementation::collapse( const QModelIndex& index )
{
	auto it = std::find( expanded_.begin(), expanded_.end(), index );
	if (it != expanded_.end())
	{
		std::swap( 
			expanded_[ it - expanded_.begin() ], 
			expanded_[ expanded_.size() - 1 ] );
		expanded_.pop_back();
	}
}


bool TreeExtension::Implementation::expanded( const QModelIndex& index ) const
{
	return 
		std::find( expanded_.cbegin(), expanded_.cend(), index ) != 
		expanded_.cend();
}


TreeExtension::TreeExtension()
	: impl_( new Implementation( *this ) )
{
}


TreeExtension::~TreeExtension()
{
}


QHash< int, QByteArray > TreeExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	this->registerRole( ItemRole::childModelName, roleNames );
	this->registerRole( ItemRole::hasChildrenName, roleNames );
	this->registerRole( ItemRole::expandedName, roleNames );
	return roleNames;
}


QVariant TreeExtension::data( const QModelIndex &index, int role ) const
{
	auto model = index.model();
	assert( model != nullptr );

	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId == ItemRole::childModelId)
	{
		if (!model->hasChildren(index) ||
			!impl_->expanded( index ))
		{
			return QVariant( QVariant::Invalid );
		}

		auto it = std::find( impl_->childModels_.begin(), 
			impl_->childModels_.end(), index );
		if (it != impl_->childModels_.end())
		{
			return QVariant::fromValue< QAbstractItemModel* >( it->data_.get() );
		}
		else
		{
			auto pChildModel = new ChildListAdapter( index );
			impl_->childModels_.emplace_back( index, pChildModel );
			return QVariant::fromValue< QAbstractItemModel* >( pChildModel );
		}
	}
	else if (roleId == ItemRole::hasChildrenId)
	{
		return model->hasChildren( index );
	}
	else if (roleId == ItemRole::expandedId)
	{
		return impl_->expanded( index );
	}

	return QVariant( QVariant::Invalid );
}


bool TreeExtension::setData( 
	const QModelIndex &index, const QVariant &value, int role )
{
	auto model = index.model();
	assert( model != nullptr );

	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return false;
	}

	if (roleId == ItemRole::expandedId)
	{
		// Change the data
		auto expand = value.toBool();
		if (impl_->expanded( index ) == expand)
		{
			return false;
		}

		expand ? impl_->expand( index ) : impl_->collapse( index );

		// Emit the data change
		QVector< int > roles;
		roles.append( role );
		emit const_cast< QAbstractItemModel * >( model )->dataChanged( index, index, roles );

		return true;
	}

	return false;
}


void TreeExtension::onLayoutAboutToBeChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		isolateRedundantIndices( 
			*it, impl_->childModels_, impl_->redundantChildModels_ );
	}
}


void TreeExtension::onLayoutChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	impl_->redundantChildModels_.clear();

	QVector< int > roles;
	int role;
	auto res = this->encodeRole( ItemRole::childModelId, role );
	assert( res );
	roles.append( role );
	res = this->encodeRole( ItemRole::hasChildrenId, role );
	assert( res );
	roles.append( role );
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		auto model = it->model();
		assert( model != nullptr );

		emit const_cast< QAbstractItemModel * >( model )->dataChanged( *it, *it, roles );
	}
}


void TreeExtension::onRowsAboutToBeRemoved( 
	const QModelIndex& parent, int first, int last )
{
	isolateRedundantIndices( parent, first, last,
		impl_->childModels_, impl_->redundantChildModels_ );
}


void TreeExtension::onRowsRemoved(
	const QModelIndex & parent, int first, int last )
{
	impl_->redundantChildModels_.clear();
}


QItemSelection TreeExtension::itemSelection( const QModelIndex & first, const QModelIndex & last ) const
{
	if (!first.isValid() && !last.isValid())
	{
		return QItemSelection();
	}
	if (!first.isValid() && last.isValid())
	{
		return QItemSelection( last, last );
	}
	if (first.isValid() && !last.isValid())
	{
		return QItemSelection( first, first );
	}

	assert( first.column() == 0 && !first.parent().isValid() );
	assert( last.column() == 0 && !last.parent().isValid() );

	if (first.row() > last.row())
	{
		return QItemSelection( last, first );
	}
	return QItemSelection( first, last );
}