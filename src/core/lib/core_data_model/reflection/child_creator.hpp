#ifndef CHILD_CREATOR_HPP
#define CHILD_CREATOR_HPP

#include "property_model_extensions.hpp"

#include "core_common/signal.hpp"

#include <set>

class ChildCreator
{
public:
    ChildCreator(IDefinitionManager& defMng);
    ~ChildCreator();

    const PropertyNode* createRoot(const ObjectHandle& handle);
    void updateSubTree(const PropertyNode* parent);
    void removeNode(const PropertyNode* parent);
    void clear();

    void registerExtension(ChildCreatorExtension* extension);
    void unregisterExtension(ChildCreatorExtension* extension);

    ::Signal<void(const PropertyNode* parent, const PropertyNode* child, size_t childPosition)> nodeCreated;
    ::Signal<void(const PropertyNode* child)> nodeRemoved;

private:
    ChildCreatorExtension * extensions = ChildCreatorExtension::createDummy();
    IDefinitionManager& definitionManager;

    std::unordered_map<const PropertyNode *, std::vector<const PropertyNode*>> propertiesIndex;
    std::shared_ptr<IChildAllocator> allocator;
};

#endif