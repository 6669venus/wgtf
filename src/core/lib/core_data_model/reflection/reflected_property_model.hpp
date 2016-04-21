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

class ReflectedPropertyModel;
class RefPropertyItem: public IItem
{
public:
    RefPropertyItem(ReflectedPropertyModel & model);

    RefPropertyItem(const RefPropertyItem& other) = delete;
    RefPropertyItem(RefPropertyItem&& other) = delete;
    RefPropertyItem& operator=(const RefPropertyItem& other) = delete;
    RefPropertyItem& operator=(RefPropertyItem&& other) = delete;

    const char * getDisplayText(int column) const override;
    ThumbnailData getThumbnail(int column) const override;
    Variant getData(int column, size_t roleId) const override;
    bool setData(int column, size_t roleId, const Variant & data) override;

    const std::string& getIndexPath() const;
    IBasePropertyPtr getProperty() const;
    Variant getValue() const;

private:
    friend class ReflectedPropertyModel;
    RefPropertyItem * getParent() const;
    size_t getPosition() const;
    RefPropertyItem * createChild();

    size_t getChildCount() const;
    RefPropertyItem * getChild(size_t index) const;
    void removeChild(size_t index);
    void removeChildren();

    void addObject(const PropertyNode* node);
    void removeObject(const PropertyNode* node);
    Variant evalValue(IDefinitionManager & definitionManager) const;

    RefPropertyItem(RefPropertyItem * parent, size_t position);

private:
    ReflectedPropertyModel & model;
    RefPropertyItem * parent = nullptr;
    size_t position = 0;
    std::vector<std::unique_ptr<RefPropertyItem>> children;
    std::vector<const PropertyNode*> nodes;
    Variant itemValue;
    mutable std::string indexPath;
};

class ReflectedPropertyModel: public ITreeModel
{
public:
    ReflectedPropertyModel(IDefinitionManager & definitionManager,
                           ICommandManager & commandManager);
    ~ReflectedPropertyModel();

    void update();

    void setObjects(const std::vector<ObjectHandle>& objects);
    void addObjects(const std::vector<ObjectHandle>& objects);
    void removeObjects(const std::vector<ObjectHandle>& objects);

    IItem * item(size_t index, const IItem * parent) const override;
    ItemIndex index(const IItem * item) const override;
    size_t size(const IItem * item) const override;
    int columnCount() const override;

    void registerExtension(GetterExtension * extension);
    void unregisterExtension(GetterExtension * extension);

    void registerExtension(SetterExtension * extension);
    void unregisterExtension(SetterExtension * extension);

    void registerExtension(ChildCreatorExtension * extension);
    void unregisterExtension(ChildCreatorExtension * extension);

private:
    void childPositionChanged(const PropertyNode* node, size_t childPosition);
    void childAdded(const PropertyNode* parent, const PropertyNode* node, size_t childPosition);
    void childRemoved(const PropertyNode* node);

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
    std::map<const PropertyNode*, RefPropertyItem*> nodeToItem;

    GetterExtension * getterExtension = nullptr;
    SetterExtension * setterExtension = nullptr;
    ChildCreator childCreator;
};

#endif