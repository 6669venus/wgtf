#include "particle_node_tree_model.hpp"
#include "particle_node_tree_item.hpp"

#include "data_model/i_item_role.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
#include <memory>
#include <thread>

struct ParticleNodeTreeModel::Implementation
{
	Implementation(ParticleNodeTreeModel& main);
	~Implementation();

	std::vector<ParticleNodeTreeItem*> getSection(const ParticleNodeTreeItem* parent);
	char* copyString(const std::string& s) const;
	void generateData();
	ParticleNodeTreeItem* AddItem(const ParticleNodeTreeItem* parent, const std::string& name, size_t level);

	ParticleNodeTreeModel& main_;
	std::unordered_map<const ParticleNodeTreeItem*, std::vector<ParticleNodeTreeItem*>> data_;
};

ParticleNodeTreeModel::Implementation::Implementation(ParticleNodeTreeModel& main)
	: main_(main)
{
	generateData();
}

ParticleNodeTreeModel::Implementation::~Implementation()
{
	for( auto itr = data_.begin(); itr != data_.end(); ++itr )
	{
		auto items = itr->second;
		size_t max = items.size();

		for( size_t i = 0; i < max; ++i )
		{
			delete items[i];
		}
	}

	data_.clear();
}

std::vector<ParticleNodeTreeItem*> ParticleNodeTreeModel::Implementation::getSection(const ParticleNodeTreeItem* parent)
{
	auto itr = data_.lower_bound(parent);
	assert(itr != data_.end());
	return itr->second;
}

char* ParticleNodeTreeModel::Implementation::copyString(const std::string& s) const
{
	char* itemData = new char[s.length() + 1];
	memcpy(itemData, s.data(), s.length());
	itemData[s.length()] = 0;
	return itemData;
}

void ParticleNodeTreeModel::Implementation::generateData()
{
	{
		auto root = AddItem(nullptr, "Emitter", 0);

		AddItem(root, "Spawn", 1);
		AddItem(root, "Actions", 1);
		AddItem(root, "Events", 1);

		auto forces = AddItem(root, "Forces", 1);
		AddItem(forces, "Direction", 2);
		AddItem(forces, "Drag", 2);
		AddItem(forces, "Point Gravity", 2);
		AddItem(forces, "Spin", 2);
		AddItem(forces, "Randomize Velocity", 2);
	}
	{
		auto root = AddItem(nullptr, "GPU Emitter", 0);

		AddItem(root, "Spawn", 1);
		AddItem(root, "Actions", 1);
		AddItem(root, "Events", 1);

		auto forces = AddItem(root, "Forces", 1);
		AddItem(forces, "Direction", 2);
		AddItem(forces, "Velocity Field", 2);
	}	
	{
		auto root = AddItem(nullptr, "Trail Emitter", 0);

		AddItem(root, "Spawn", 1);
		AddItem(root, "Actions", 1);
		AddItem(root, "Events", 1);

		auto forces = AddItem(root, "Forces", 1);
		AddItem(forces, "Direction", 2);
		AddItem(forces, "Beam Noise", 2);
	}
}

ParticleNodeTreeItem* ParticleNodeTreeModel::Implementation::AddItem(const ParticleNodeTreeItem* parent, const std::string& name, size_t level)
{
	auto item = new ParticleNodeTreeItem(copyString(name), parent);
	data_[parent].push_back(item);
	data_[item];

	return item;
}

ParticleNodeTreeModel::ParticleNodeTreeModel()
	: impl_(new Implementation(*this))
{
}

ParticleNodeTreeModel::ParticleNodeTreeModel(const ParticleNodeTreeModel& rhs)
	: impl_(new Implementation(*this))
{
}

ParticleNodeTreeModel::~ParticleNodeTreeModel()
{
}

ParticleNodeTreeModel& ParticleNodeTreeModel::operator=(const ParticleNodeTreeModel& rhs)
{
	if( this != &rhs )
	{
		impl_.reset(new Implementation(*this));
	}

	return *this;
}

IItem* ParticleNodeTreeModel::item(size_t index, const IItem* parent) const
{
	auto temp = static_cast<const ParticleNodeTreeItem*>(parent);
	return impl_->getSection(temp)[index];
}

ITreeModel::ItemIndex ParticleNodeTreeModel::index(const IItem* item) const
{
	auto temp = static_cast<const ParticleNodeTreeItem*>(item);
	temp = static_cast<const ParticleNodeTreeItem*>(temp->getParent());
	ItemIndex index(0, temp);

	auto items = impl_->getSection(temp);
	auto itr = std::find(items.begin(), items.end(), item);
	assert(itr != items.end());

	index.first = itr - items.begin();
	return index;
}


bool ParticleNodeTreeModel::empty(const IItem* parent) const
{
	const auto temp = static_cast<const ParticleNodeTreeItem*>(parent);
	return impl_->getSection(temp).empty();
}


size_t ParticleNodeTreeModel::size(const IItem* parent) const
{
	auto temp = static_cast<const ParticleNodeTreeItem*>(parent);
	return impl_->getSection(temp).size();
}
