#ifndef I_PROPERTY_MODEL_EXTENSION_HPP
#define I_PROPERTY_MODEL_EXTENSION_HPP

#include "core_variant/variant.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/object_handle.hpp"

class ICommandManager;
class IDefinitionManager;
class RefPropertyItem;

struct PropertyNode
{
    enum PropertyType: int32_t
    {
        Invalid = -1,
        SelfRoot = 0,
        RealProperty,
        CollectionItem,
        GroupProperty,
        VirtualProperty, // reserve some range for generic types. I don't know what types it will be now,
                             // but reserve some values is good idea in my opinion
        DomainSpecificProperty = 255
        // use can use values DomainSpecificProperty, DomainSpecificProperty + 1, ... , DomainSpecificProperty + n
        // on you own purpose. It's only way to transfer some information between iterations
    };

    PropertyNode(IBasePropertyPtr property, ObjectHandle object);
    PropertyNode(int propertyType, IBasePropertyPtr property, ObjectHandle object);

    bool operator== (const PropertyNode& other) const;
    bool operator!= (const PropertyNode& other) const;
    bool operator< (const PropertyNode& other) const;

    int propertyType; // it can be value from PropertyType or any value that you set in your extension
    IBasePropertyPtr propertyInstance;
    ObjectHandle object; // this object should be always object that can be transfer into IBasePropertyPtr::setValue() to set new value.
};

class IChildAllocator
{
public:
    virtual ~IChildAllocator() {}

    virtual const PropertyNode* createPropertyNode(IBasePropertyPtr propertyInstance, ObjectHandle object, int type = PropertyNode::RealProperty) = 0;
    virtual void deletePropertyNode(const PropertyNode* node) = 0;

    virtual IBasePropertyPtr getCollectionItemProperty(std::string&& name, const TypeId& type, IDefinitionManager & defMng) = 0;
};

const PropertyNode* MakeRootNode(ObjectHandle handle, IChildAllocator& allocator);

template<typename T>
class ExtensionChain
{
public:
    virtual ~ExtensionChain()
    {
        assert(nextExtension == nullptr);
    }

    T* addExtension(T* extension)
    {
        static_assert(std::is_base_of<ExtensionChain<T>, T>::value, "Extension should inherit IExtensionChain");
        extension->nextExtension = dynamic_cast<T *>(this);
        assert(extension->nextExtension != nullptr);
        return extension;
    }

    T* removeExtension(T* extension)
    {
        static_assert(std::is_base_of<ExtensionChain<T>, T>::value, "Extension should inherit IExtensionChain");
        if (this == extension)
        {
            T * result = extension->nextExtension;
            extension->nextExtension = nullptr;
            return result;
        }

        nextExtension = nextExtension->removeExtension(extension);
        T * result = extension->nextExtension = dynamic_cast<T *>(this);
        assert(result != nullptr);
        return result;
    }

    static void deleteExtensionChain(T * chain)
    {
        static_assert(std::is_base_of<ExtensionChain<T>, T>::value, "Extension should inherit IExtensionChain");
        while (chain != nullptr)
        {
            T * toDelete = chain;
            chain = chain->removeExtension(chain);
            delete toDelete;
        }
    }

protected:
    T * nextExtension = nullptr;
};

class ChildCreatorExtension: public ExtensionChain<ChildCreatorExtension>
{
public:

    // return true if extension was applied, and ChildCreator should not try to apply next registered extension
    // return false means : "not my type, i don't know what to do with this"
    virtual void exposeChildren(const PropertyNode& node, std::vector<const PropertyNode*> & children, IDefinitionManager& defMng) const;
    static ChildCreatorExtension* createDummy();

    void setAllocator(std::shared_ptr<IChildAllocator> allocator);

protected:
    std::shared_ptr<IChildAllocator> allocator;
};

class GetterExtension: public ExtensionChain<GetterExtension>
{
public:
    virtual Variant getValue(const RefPropertyItem* item, int column, size_t roleId, IDefinitionManager & definitionManager) const;
    static GetterExtension* createDummy();
};

class SetterExtension: public ExtensionChain<SetterExtension>
{
public:
    virtual bool setValue(RefPropertyItem * item, int column, size_t roleId, const Variant & data,
                          IDefinitionManager & definitionManager, ICommandManager & commandManager) const;
    static SetterExtension* createDummy();
};

class MergeValuesExtension: public ExtensionChain<MergeValuesExtension>
{
public:
    virtual RefPropertyItem* lookUpItem(const PropertyNode* node, const std::vector<std::unique_ptr<RefPropertyItem>>& items,
                                        IDefinitionManager & definitionManager) const;
    static MergeValuesExtension* createDummy();
};

class InjectDataExtension: public ExtensionChain<InjectDataExtension>
{
public:
    virtual void inject(RefPropertyItem* item);
    virtual void updateInjection(RefPropertyItem* item);
    static InjectDataExtension* createDummy();
};

#endif