#ifndef PARTICLE_NODE_TREE_MODEL_HPP
#define PARTICLE_NODE_TREE_MODEL_HPP

#include "data_model/i_tree_model.hpp"

#include <memory>

class ParticleNodeTreeModel : public ITreeModel
{
public:
	ParticleNodeTreeModel();
	ParticleNodeTreeModel(const ParticleNodeTreeModel& rhs);
	virtual ~ParticleNodeTreeModel();

	ParticleNodeTreeModel& operator=(const ParticleNodeTreeModel& rhs);

	virtual IItem* item(size_t index, const IItem* parent) const override;
	virtual ItemIndex index(const IItem* item) const override;
	virtual bool empty(const IItem* parent) const override;
	virtual size_t size(const IItem* parent) const override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // PARTICLE_NODE_TREE_MODEL_HPP
