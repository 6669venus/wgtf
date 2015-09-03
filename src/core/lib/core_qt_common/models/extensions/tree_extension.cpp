#include "tree_extension.hpp"
#include "core_qt_common/models/adapters/child_list_adapter.hpp"
#include "core_qt_common/models/adapters/indexed_adapter.hpp"

struct TreeExtension::Implementation
{
	void expand( const QModelIndex& index );
	void collapse( const QModelIndex& index );
	bool expanded( const QModelIndex& index ) const;
	void purgeExpanded();

	std::vector< IndexedAdapter< ChildListAdapter > > childModels_;
	std::vector< std::unique_ptr< ChildListAdapter > > redundantChildModels_;
	std::vector< QPersistentModelIndex > expanded_;

	QModelIndex currentIndex_;
};


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


void TreeExtension::Implementation::purgeExpanded()
{
	auto predicate = []( const QPersistentModelIndex & index ) 
	{ 
		return !index.isValid(); 
	};
	expanded_.erase(	
		std::remove_if( expanded_.begin(), expanded_.end(), predicate),
		expanded_.end() );
}


TreeExtension::TreeExtension()
	: impl_( new Implementation() )
{
}


TreeExtension::~TreeExtension()
{
}


QHash< int, QByteArray > TreeExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	this->registerRole( ChildModelRole::role_, roleNames );
	this->registerRole( HasChildrenRole::role_, roleNames );
	this->registerRole( ExpandedRole::role_, roleNames );
	this->registerRole( ParentIndexRole::role_, roleNames );
	return roleNames;
}


QVariant TreeExtension::data( const QModelIndex &index, int role ) const
{
	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId == ChildModelRole::roleId_)
	{
		if (!model_->hasChildren(index) ||
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
	else if (roleId == HasChildrenRole::roleId_)
	{
		return model_->hasChildren( index );
	}
	else if (roleId == ExpandedRole::roleId_)
	{
		return impl_->expanded( index );
	}
	else if (roleId == ParentIndexRole::roleId_)
	{
		QModelIndex parentIndex = model_->parent( index );
		return parentIndex;
	}

	return QVariant( QVariant::Invalid );
}


bool TreeExtension::setData( 
	const QModelIndex &index, const QVariant &value, int role )
{
	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return false;
	}

	if (roleId == ExpandedRole::roleId_)
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
		// The child model role is dependent on the expanded role
		auto res = this->encodeRole( ChildModelRole::roleId_, role );
		assert( res );
		roles.append( role );
		emit model_->dataChanged( index, index, roles );

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
	impl_->purgeExpanded();

	QVector< int > roles;
	int role;
	auto res = this->encodeRole( ChildModelRole::roleId_, role );
	assert( res );
	roles.append( role );
	res = this->encodeRole( HasChildrenRole::roleId_, role );
	assert( res );
	roles.append( role );
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		emit model_->dataChanged( *it, *it, roles );
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
	impl_->purgeExpanded();
}


/// Move to previous index
void TreeExtension::moveUp()
{
	int prevRow = impl_->currentIndex_.row() - 1;

	if (0 <= prevRow)
	{
		// See if the previous 
		QModelIndex prevIndex = impl_->currentIndex_.sibling( prevRow, 0 );

		// See if the previous item is expanded
		if (impl_->expanded( prevIndex ))
		{
			// Previous item's bottom row
			int prevIndexsBottomRow = model_->rowCount( prevIndex ) - 1;
			impl_->currentIndex_ = prevIndex.child( prevIndexsBottomRow, 0 );
		}
		else
		{
			// Update the current index if the previous item is not expanded
			impl_->currentIndex_ = prevIndex;
		}

		emit currentIndexChanged();
	}
	else
	{
		QModelIndex parent = impl_->currentIndex_.parent();

		if (parent.isValid())
		{
			// Update the current index if the parent is valid
			impl_->currentIndex_ = parent;
			emit currentIndexChanged();
		}
	}
}


/// Move to next index
void TreeExtension::moveDown()
{
	if (impl_->expanded( impl_->currentIndex_ ))
	{
		// Navigate to the child tree when the current item is expanded
		impl_->currentIndex_ = impl_->currentIndex_.child( 0, 0 );
		emit currentIndexChanged();
	}
	else
	{
		QModelIndex parent = impl_->currentIndex_.parent();

		if (parent.isValid())
		{
			int nextRow = impl_->currentIndex_.row() + 1;

			if (nextRow < model_->rowCount( parent ))
			{
				// Update the current index if the next item is available
				impl_->currentIndex_ = impl_->currentIndex_.sibling( nextRow, 0 );
				emit currentIndexChanged();
			}
			else
			{
				// Reached the bottom, see if the parent has more items
				nextRow = parent.row() + 1;
				if (nextRow < model_->rowCount( parent.parent() ))
				{
					impl_->currentIndex_ = parent.sibling( nextRow, 0 );
					emit currentIndexChanged();
				}
			}
		}
	}
}


/// Expand the current item
void TreeExtension::expand()
{
	// Make sure the current item has children and collapsed
	if (model_->hasChildren( impl_->currentIndex_ ) && !impl_->expanded( impl_->currentIndex_ ))
	{
		int expandedRole = -1;
		this->encodeRole( ExpandedRole::roleId_, expandedRole );

		setData( impl_->currentIndex_, QVariant( true ), expandedRole );
	}
}


/// Collapse the current item
void TreeExtension::collapse()
{
	// Make sure the current item has children and expanded
	if (model_->hasChildren( impl_->currentIndex_ ) && impl_->expanded( impl_->currentIndex_ ))
	{
		int expandedRole = -1;
		this->encodeRole( ExpandedRole::roleId_, expandedRole );

		setData( impl_->currentIndex_, QVariant( false ), expandedRole );
	}
}


QVariant TreeExtension::getCurrentIndex() const
{
	return QVariant::fromValue( impl_->currentIndex_ );
}


void TreeExtension::setCurrentIndex( const QVariant& index )
{
	QModelIndex idx = index.toModelIndex();
	impl_->currentIndex_ = idx;

	emit currentIndexChanged();
}