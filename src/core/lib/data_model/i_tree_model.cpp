#include "i_tree_model.hpp"

IItem * ITreeModel::item( ItemIndex index ) const
{
	return this->item( index.first, index.second );
}

bool ITreeModel::empty( const IItem * item ) const
{
	return (this->size( item ) == 0);
}
