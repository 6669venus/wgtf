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

    std::shared_ptr<const PropertyNode> createRoot(const ObjectHandle& handle);
    void updateSubTree(const std::shared_ptr<const PropertyNode>& parent);
    void removeNode(const std::shared_ptr<const PropertyNode>& parent);
    void clear();

    void registerExtension(ChildCreatorExtension* extension);
    void unregisterExtension(ChildCreatorExtension* extension);

    ::Signal<void(std::shared_ptr<const PropertyNode> parent, std::shared_ptr<const PropertyNode> child, size_t childPosition)> nodeCreated;
    ::Signal<void(std::shared_ptr<const PropertyNode> child)> nodeRemoved;

private:
    ChildCreatorExtension * extensions = ChildCreatorExtension::createDummy();
    IDefinitionManager& definitionManager;

    std::unordered_map<std::shared_ptr<const PropertyNode>, std::vector<std::shared_ptr<const PropertyNode>>> propertiesIndex;
    std::shared_ptr<IChildAllocator> allocator;
};

#endif