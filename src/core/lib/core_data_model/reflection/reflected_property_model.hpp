#ifndef REFLECTED_PROPERTY_MODEL_HPP
#define REFLECTED_PROPERTY_MODEL_HPP

#include "child_creator.hpp"

#include "core_data_model/reflection/property_model_extensions.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item.hpp"

#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/object_handle.hpp"

class ICommandManager;
class IDefinitionManager;
class IReflectionController;
class ReflectedPropertyModel;

class RefPropertyItem: public IItem
{
public:
    RefPropertyItem(ReflectedPropertyModel & model);

    const char * getDisplayText(int column) const override;
    ThumbnailData getThumbnail(int column) const override;
    Variant getData(int column, size_t roleId) const override;
    bool setData(int column, size_t roleId, const Variant & data) override;

    const std::string& getIndexPath() const;
    IBasePropertyPtr getProperty() const;
    Variant getValue() const;

    const std::vector<std::shared_ptr<const PropertyNode>>& getObjects() const { return nodes; }

    void injectData(size_t roleId, const Variant& value);
    Variant getInjectedData(size_t roleId);

    const RefPropertyItem * getParent() const;

private:
    //![don't call it]
    RefPropertyItem(const RefPropertyItem& other) : model(other.model) {}
    RefPropertyItem(RefPropertyItem&& other) : model(other.model) {}
    RefPropertyItem& operator=(const RefPropertyItem& other) { return *this; }
    RefPropertyItem& operator=(RefPropertyItem&& other) { return *this; }
    //![don't call it]

    friend class ReflectedPropertyModel;
    RefPropertyItem * getNonConstParent() const;
    size_t getPosition() const;
    RefPropertyItem * createChild();

    size_t getChildCount() const;
    RefPropertyItem * getChild(size_t index) const;
    void removeChild(size_t index);
    void removeChildren();

    void addObject(const std::shared_ptr<const PropertyNode>& node);
    void removeObject(const std::shared_ptr<const PropertyNode>& node);
    void removeObjects();
    bool hasObjects() const;

    Variant evalValue(IDefinitionManager & definitionManager) const;
    void setValue(Variant&& newValue);

    RefPropertyItem(RefPropertyItem * parent, size_t position);

private:
    ReflectedPropertyModel & model;
    RefPropertyItem * parent;
    size_t position;
    std::vector<std::unique_ptr<RefPropertyItem>> children;
    std::vector<std::shared_ptr<const PropertyNode>> nodes;
    Variant itemValue;
    mutable std::string indexPath;

    std::unordered_map<size_t, Variant> injectedData;
};

class ReflectedPropertyModel: public ITreeModel
{
public:
    ReflectedPropertyModel(IDefinitionManager & definitionManager,
                           ICommandManager & commandManager, IReflectionController& refController);
    ~ReflectedPropertyModel();

    void update();

    void setObjects(const std::vector<ObjectHandle>& objects);

    IItem * item(size_t index, const IItem * parent) const override;
    ItemIndex index(const IItem * item) const override;
    size_t size(const IItem * item) const override;
    int columnCount() const override;

    void registerExtension(GetterExtension* extension);
    void unregisterExtension(GetterExtension* extension);

    void registerExtension(SetterExtension* extension);
    void unregisterExtension(SetterExtension* extension);

    void registerExtension(MergeValuesExtension* extension);
    void unregisterExtension(MergeValuesExtension* extension);

    void registerExtension(ChildCreatorExtension* extension);
    void unregisterExtension(ChildCreatorExtension* extension);

    void registerExtension(InjectDataExtension* extension);
    void unregisterExtension(InjectDataExtension* extension);

private:
    void childAdded(const std::shared_ptr<const PropertyNode>& parent, const std::shared_ptr<const PropertyNode>& node, size_t childPosition);
    void childRemoved(const std::shared_ptr<const PropertyNode>& node);

    const RefPropertyItem* getEffectiveParent(const IItem* modelParent) const;
    RefPropertyItem* getEffectiveParent(IItem* modelParent) const;

    const IItem* getModelParent(const RefPropertyItem* effectiveParent) const;
    IItem* getModelParent(RefPropertyItem* effectiveParent) const;

    void update(RefPropertyItem* item);

private:
    friend class RefPropertyItem;
    Variant getData(const RefPropertyItem * item, int column, size_t roleId) const;
    bool setData(RefPropertyItem * item, int column, size_t roleId, const Variant & data);

    IDefinitionManager & definitionManager;
    ICommandManager & commandManager;
    std::unique_ptr<RefPropertyItem> rootItem;
    std::map<std::shared_ptr<const PropertyNode>, RefPropertyItem*> nodeToItem;

    GetterExtension* getterExtension;
    SetterExtension* setterExtension;
    MergeValuesExtension* mergeExtension;
    InjectDataExtension* injectExtension;
    ChildCreator childCreator;
};

#endif