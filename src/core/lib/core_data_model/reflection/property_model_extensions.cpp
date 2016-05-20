#include "property_model_extensions.hpp"

#include "core_reflection/metadata/meta_types.hpp"
#include "core_variant/type_id.hpp"

namespace PMEDetails
{

class SelfProperty: public IBaseProperty
{
public:
    const TypeId & getType() const override
    {
        return TypeId::getType<ObjectHandle>();
    }

    const char * getName() const override
    {
        return "Self";
    }

    uint64_t getNameHash() const override
    {
        static uint64_t hash = HashUtilities::compute(getName());
        return hash;
    }

    MetaHandle getMetaData() const override
    {
        return MetaNone();
    }

    bool readOnly() const override
    {
        return true;
    }

    bool isMethod() const override
    {
        return false;
    }

    bool isValue() const override
    {
        return true;
    }

    bool set(const ObjectHandle&, const Variant&, const IDefinitionManager&) const override
    {
        return false;
    }

    Variant get(const ObjectHandle& handle, const IDefinitionManager&) const override
    {
        return Variant(handle);
    }

    size_t parameterCount() const override
    {
        return 0;
    }

    Variant invoke(const ObjectHandle&, const ReflectedMethodParameters&) { return Variant(); }
};

class DummyChildCreator : public ChildCreatorExtension
{
public:
    void exposeChildren(const std::shared_ptr<const PropertyNode>&, std::vector<std::shared_ptr<const PropertyNode>>&, IDefinitionManager&) const override {}
};

class DummyGetter: public GetterExtension
{
public:
    Variant getValue(const RefPropertyItem *, int, size_t, IDefinitionManager&) const override
    {
        return Variant();
    }
};

class DummySetter: public SetterExtension
{
public:
    bool setValue(RefPropertyItem*, int, size_t, const Variant&, IDefinitionManager&, ICommandManager&) const override
    {
        return false;
    }
};

class DummyMerger: public MergeValuesExtension
{
public:
    RefPropertyItem* lookUpItem(const std::shared_ptr<const PropertyNode>& node, const std::vector<std::unique_ptr<RefPropertyItem>>& items,
                                IDefinitionManager & definitionManager) const override
    {
        return nullptr;
    }
};

class DummyInjector: public InjectDataExtension
{
public:
    void inject(RefPropertyItem* item) override{}
    void updateInjection(RefPropertyItem * item) override{}
};

IBasePropertyPtr selfRootProperty = std::make_shared<SelfProperty>();
}

PropertyNode::PropertyNode()
    : propertyType(-1)
{
}

bool PropertyNode::operator==(const PropertyNode & other) const
{
    return propertyType == other.propertyType &&
        propertyInstance == other.propertyInstance &&
        object == other.object;
}

bool PropertyNode::operator!=(const PropertyNode & other) const
{
    return propertyType != other.propertyType ||
        propertyInstance != other.propertyInstance ||
        object != other.object;
}

bool PropertyNode::operator<(const PropertyNode & other) const
{
    if (propertyType != other.propertyType)
        return propertyType < other.propertyType;
    if (propertyInstance != other.propertyInstance)
        return propertyInstance < other.propertyInstance;

    return object < other.object;
}

std::shared_ptr<const PropertyNode> MakeRootNode(ObjectHandle handle, IChildAllocator& allocator)
{
    return allocator.createPropertyNode(PMEDetails::selfRootProperty, handle, PropertyNode::SelfRoot);
}

void ChildCreatorExtension::exposeChildren(const std::shared_ptr<const PropertyNode>& node, std::vector<std::shared_ptr<const PropertyNode>>& children, IDefinitionManager & defMng) const
{
    assert(nextExtension != nullptr);
    nextExtension->exposeChildren(node, children, defMng);
}

ChildCreatorExtension * ChildCreatorExtension::createDummy()
{
    return new PMEDetails::DummyChildCreator();
}

void ChildCreatorExtension::setAllocator(std::shared_ptr<IChildAllocator> allocator_)
{
    allocator = allocator_;
}

Variant GetterExtension::getValue(const RefPropertyItem * item, int column, size_t roleId, IDefinitionManager & definitionManager) const
{
    assert(nextExtension != nullptr);
    return nextExtension->getValue(item, column, roleId, definitionManager);
}

GetterExtension * GetterExtension::createDummy()
{
    return new PMEDetails::DummyGetter();
}

bool SetterExtension::setValue(RefPropertyItem* item, int column, size_t roleId, const Variant& data, IDefinitionManager& definitionManager, ICommandManager& commandManager) const
{
    assert(nextExtension != nullptr);
    return nextExtension->setValue(item, column, roleId, data, definitionManager, commandManager);
}

SetterExtension * SetterExtension::createDummy()
{
    return new PMEDetails::DummySetter();
}

RefPropertyItem * MergeValuesExtension::lookUpItem(const std::shared_ptr<const PropertyNode>& node, const std::vector<std::unique_ptr<RefPropertyItem>>& items,
                                                   IDefinitionManager & definitionManager) const
{
    assert(nextExtension != nullptr);
    return nextExtension->lookUpItem(node, items, definitionManager);
}

MergeValuesExtension * MergeValuesExtension::createDummy()
{
    return new PMEDetails::DummyMerger();
}

void InjectDataExtension::inject(RefPropertyItem* item)
{
    assert(nextExtension != nullptr);
    return nextExtension->inject(item);
}

void InjectDataExtension::updateInjection(RefPropertyItem * item)
{
    assert(nextExtension != nullptr);
    return nextExtension->updateInjection(item);
}

InjectDataExtension* InjectDataExtension::createDummy()
{
    return new PMEDetails::DummyInjector();
}
