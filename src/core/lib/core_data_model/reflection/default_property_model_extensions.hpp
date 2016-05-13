#ifndef DEFAULT_PROPERTY_MODEL_EXTENSION
#define DEFAULT_PROPERTY_MODEL_EXTENSION

#include "property_model_extensions.hpp"

class IDefinitionManager;
class IReflectionController;
class DefaultGetterExtension : public GetterExtension
{
public:
    Variant getValue(const RefPropertyItem* item, int column, size_t roleId, IDefinitionManager& defMng) const override;
};

class UrlGetterExtension: public GetterExtension
{
public:
    Variant getValue(const RefPropertyItem* item, int column, size_t roleId, IDefinitionManager& defMng) const override;
};

class DefaultSetterExtension: public SetterExtension
{
public:
    DefaultSetterExtension(IReflectionController& reflectionController);

    bool setValue(RefPropertyItem * item, int column, size_t roleId, const Variant & data,
                  IDefinitionManager & definitionManager, ICommandManager & commandManager) const;

private:
    IReflectionController& controller;
};

class DefaultChildCheatorExtension: public ChildCreatorExtension
{
public:
    void exposeChildren(const PropertyNode& node, std::vector<const PropertyNode*> & children, IDefinitionManager& defMng) const override;
};

class DefaultMergeValueExtension: public MergeValuesExtension
{
public:
    RefPropertyItem* lookUpItem(const PropertyNode* node, const std::vector<std::unique_ptr<RefPropertyItem>>& items,
                                IDefinitionManager & definitionManager) const override;
};

std::shared_ptr<IChildAllocator> createDefaultAllocator();

#endif
