#ifndef DEFAULT_PROPERTY_MODEL_EXTENSION
#define DEFAULT_PROPERTY_MODEL_EXTENSION

#include "property_model_extensions.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{

class IDefinitionManager;
class IReflectionController;
class DefaultSetterGetterExtension: public SetterGetterExtension
{
public:
    DefaultSetterGetterExtension(IComponentContext& context);
    Variant getValue(const RefPropertyItem* item, int column, size_t roleId, IDefinitionManager& defMng) const override;
    bool setValue(RefPropertyItem * item, int column, size_t roleId, const Variant & data,
        IDefinitionManager & definitionManager, ICommandManager & commandManager) const;

private:
    Depends<IReflectionController> reflectionControllerHolder;
};

class UrlGetterExtension: public SetterGetterExtension
{
public:
    Variant getValue(const RefPropertyItem* item, int column, size_t roleId, IDefinitionManager& defMng) const override;
};

class DefaultChildCheatorExtension: public ChildCreatorExtension
{
public:
    void exposeChildren(const std::shared_ptr<const PropertyNode>& node, std::vector<std::shared_ptr<const PropertyNode>> & children, IDefinitionManager& defMng) const override;
};

class DefaultMergeValueExtension: public MergeValuesExtension
{
public:
    RefPropertyItem* lookUpItem(const std::shared_ptr<const PropertyNode>& node, const std::vector<std::unique_ptr<RefPropertyItem>>& items,
        IDefinitionManager & definitionManager) const override;
};

std::shared_ptr<IChildAllocator> createDefaultAllocator();

}
#endif
