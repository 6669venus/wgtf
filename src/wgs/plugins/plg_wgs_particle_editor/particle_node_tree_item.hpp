#ifndef PARTICLE_NODE_TREE_ITEM_HPP
#define PARTICLE_NODE_TREE_ITEM_HPP

#include "data_model/i_item.hpp"

#include <memory>

class ParticleNodeTreeItem: public IItem
{
public:
	ParticleNodeTreeItem( const char* name, const IItem* parent );
	ParticleNodeTreeItem( const ParticleNodeTreeItem& rhs );
	virtual ~ParticleNodeTreeItem();

	ParticleNodeTreeItem& operator=( const ParticleNodeTreeItem& rhs );

	const IItem* getParent() const;
	virtual int columnCount() const;
	virtual const char* getDisplayText( int column ) const;
	virtual ThumbnailData getThumbnail( int column ) const;
	virtual Variant getData( int column, size_t roleId ) const;
	virtual bool setData( int column, size_t roleId, const Variant& data );

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // PARTICLE_NODE_TREE_ITEM_HPP
