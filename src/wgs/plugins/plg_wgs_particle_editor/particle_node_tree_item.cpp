#include "particle_node_tree_item.hpp"

struct ParticleNodeTreeItem::Implementation
{
	Implementation(ParticleNodeTreeItem& main, const char* name, const IItem* parent);
	~Implementation();

	ParticleNodeTreeItem& main_;
	const char* name_;
	const IItem* parent_;
};

ParticleNodeTreeItem::Implementation::Implementation(ParticleNodeTreeItem& main, const char* name, const IItem* parent)
	: main_(main)
	, name_(name)
	, parent_(parent)
{
}

ParticleNodeTreeItem::Implementation::~Implementation()
{
	delete name_;
}


ParticleNodeTreeItem::ParticleNodeTreeItem(const char* name, const IItem* parent)
	: impl_(new Implementation(*this, name, parent))
{
}

ParticleNodeTreeItem::ParticleNodeTreeItem(const ParticleNodeTreeItem& rhs)
	: impl_(new Implementation(*this, rhs.impl_->name_, rhs.impl_->parent_))
{
}

ParticleNodeTreeItem::~ParticleNodeTreeItem()
{
}

ParticleNodeTreeItem& ParticleNodeTreeItem::operator=(const ParticleNodeTreeItem& rhs)
{
	if( this != &rhs )
	{
		impl_.reset(new Implementation(*this, rhs.impl_->name_, rhs.impl_->parent_));
	}

	return *this;
}

const IItem* ParticleNodeTreeItem::getParent() const
{
	return impl_->parent_;
}

int ParticleNodeTreeItem::columnCount() const
{
	return 1;
}

const char* ParticleNodeTreeItem::getDisplayText(int column) const
{
	return impl_->name_;
}

ThumbnailData ParticleNodeTreeItem::getThumbnail(int column) const
{
	return nullptr;
}

Variant ParticleNodeTreeItem::getData(int column, size_t roleId) const
{
	return Variant();
}

bool ParticleNodeTreeItem::setData(int column, size_t roleId, const Variant& data)
{
	return false;
}
