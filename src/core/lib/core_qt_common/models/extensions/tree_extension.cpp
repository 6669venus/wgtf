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

	auto begin = first;
	auto end = last;

	auto parent = QModelIndex();
	{
		// Check if end is a descendant of begin
		auto endTmp = end;
		while (endTmp.isValid())
		{
			if (begin == endTmp)
			{
				parent = begin;
				break;
			}
			endTmp = endTmp.parent();
		}
	}

	if (!parent.isValid())
	{
		// Check if begin is a descendant of end
		auto beginTmp = begin;
		while (beginTmp.isValid())
		{
			if (beginTmp == end)
			{
				std::swap(begin, end);
				parent = begin;
				break;
			}
			beginTmp = beginTmp.parent();
		}
	}

	if (!parent.isValid())
	{
		// Check if begin comes before end or vice versa
		auto beginTmp = begin;
		while (beginTmp.isValid())
		{
			auto beginParent = beginTmp.parent();
			auto endTmp = end;
			while (endTmp.isValid())
			{
				auto endParent = endTmp.parent();
				if (beginParent == endParent)
				{
					if (beginTmp.row() > endTmp.row())
					{
						std::swap(begin, end);
					}
					parent = beginParent;
					break;
				}
				endTmp = endParent;
			}
			if (parent.isValid())
			{
				break;
			}
			beginTmp = beginParent;
		}
	}

	// Create an item selection from begin to end
	QItemSelection itemSelection;

	auto it = begin;
	while (true)
	{
		itemSelection.select(it, it);

		if (it == end)
		{
			break;
		}

		// Move next
		do
		{
			if (impl_->expanded(it) &&
				it.model()->hasChildren( it ))
			{
				auto child = it.child(0, 0);
				if (child.isValid())
				{
					it = child;
					break;
				}
			}
			auto sibling = QModelIndex();
			while (it.isValid())
			{
				auto parent = it.parent();
				auto row = it.row() + 1;
				if (it.model()->rowCount( parent ) > row)
				{
					sibling = it.sibling(row, 0);
					if (sibling.isValid())
					{
						break;
					}
				}
				it = parent;
			}
			if (sibling.isValid())
			{
				it = sibling;
				break;
			}
			assert( false );
		} while (false);
		//
	}
	
	return itemSelection;
}


QModelIndex TreeExtension::getNextIndex( const QModelIndex & index ) const
{
	if (!index.isValid())
	{
		return index;
	}
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}

	// Move to next row in current scope
	if (impl_->expanded( index ) &&
		pModel->hasChildren( index ))
	{
		const auto child = index.child( 0, index.column() );
		if (child.isValid())
		{
			return child;
		}
	}

	// Move to next sibling or parent
	auto it = index;
	while (it.isValid())
	{
		const auto parent = it.parent();
		const auto row = it.row() + 1;
		if (row < it.model()->rowCount( parent ))
		{
			const auto sibling = it.sibling( row, index.column() );
			if (sibling.isValid())
			{
				return sibling;
			}
		}
		it = parent;
	}

	// Could not move to next item
	return index;
}


QModelIndex TreeExtension::getPreviousIndex( const QModelIndex & index ) const
{
	if (!index.isValid())
	{
		return index;
	}
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}

	auto it = index;
	while (it.isValid())
	{
		// Move to previous row in current scope
		const auto row = it.row() - 1;
		if (row >= 0)
		{
			const auto sibling = it.sibling( row, index.column() );
			if (sibling.isValid())
			{
				// Move to last child in the row
				if (impl_->expanded( sibling ) &&
					pModel->hasChildren( sibling ))
				{
					const int lastRow = it.model()->rowCount( sibling ) - 1;
					const auto lastChild = sibling.child( lastRow, index.column() );
					if (lastChild.isValid())
					{
						return lastChild;
					}
				}

				// Previous row does not have children expanded
				return sibling;
			}
		}

		// Move to previous parent
		it = it.parent();
		if (it.isValid())
		{
			// Move to last child in parent
			if (impl_->expanded( it ) &&
				pModel->hasChildren( it ))
			{
				const int lastRow = it.model()->rowCount( it ) - 1;
				const auto lastChild = it.child( lastRow, index.column() );
				if (lastChild.isValid())
				{
					return lastChild;
				}
			}
		}
	}

	// Could not move to previous item
	return index;
}
