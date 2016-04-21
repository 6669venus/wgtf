#include "reflected_property_model.hpp"

#include "core_data_model/reflection/default_property_model_extensions.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/base_property.hpp"

#include <set>

RefPropertyItem::RefPropertyItem(ReflectedPropertyModel & model_)
    : model(model_)
{
}

RefPropertyItem::RefPropertyItem(RefPropertyItem * parent_, size_t position_)
    : model(parent_->model)
    , parent(parent_)
    , position(position_)
{
}

const char * RefPropertyItem::getDisplayText(int column) const
{
    return getProperty()->getName();
}

ThumbnailData RefPropertyItem::getThumbnail(int column) const
{
    return nullptr;
}

Variant RefPropertyItem::getData(int column, size_t roleId) const
{
    return model.getData(this, column, roleId);
}

bool RefPropertyItem::setData(int column, size_t roleId, const Variant & data)
{
    return model.setData(this, column, roleId, data);
}

const std::string & RefPropertyItem::getIndexPath() const
{
    if (indexPath.empty())
    {
        indexPath.reserve(128);
        const RefPropertyItem* item = this;
        while (item != nullptr)
        {
            IBasePropertyPtr propertyInstance = item->getProperty();
            if (propertyInstance != nullptr)
            {
                indexPath += propertyInstance->getName();
            }

            item = static_cast<const RefPropertyItem*>(item->getParent());
        }
    }
    return indexPath;
}

IBasePropertyPtr RefPropertyItem::getProperty() const
{
    if (nodes.empty())
    {
        return nullptr;
    }

    return nodes.front()->propertyInstance;
}

Variant RefPropertyItem::getValue() const
{
    return itemValue;
}

Variant RefPropertyItem::evalValue(IDefinitionManager & definitionManager) const
{
    Variant result;
    if (!nodes.empty())
    {
        auto iter = nodes.begin();

        const PropertyNode* node = *iter;
        result = node->propertyInstance->get(node->object, definitionManager);
        ++iter;
        for (iter; iter != nodes.end(); ++iter)
        {
            const PropertyNode* node = *iter;
            Variant r = node->propertyInstance->get(node->object, definitionManager);
            if (r != result)
            {
                result = Variant();
                break;
            }
        }
    }

    return result;
}

RefPropertyItem* RefPropertyItem::getParent() const
{
    return parent;
}

size_t RefPropertyItem::getPosition() const
{
    return position;
}

RefPropertyItem * RefPropertyItem::createChild()
{
    size_t newItemindex = children.size();
    children.push_back(std::unique_ptr<RefPropertyItem>(new RefPropertyItem(this, newItemindex)));
    return children.back().get();
}

size_t RefPropertyItem::getChildCount() const
{
    return children.size();
}

RefPropertyItem * RefPropertyItem::getChild(size_t index) const
{
    assert(index < getChildCount());
    return children[index].get();
}

void RefPropertyItem::removeChild(size_t index)
{
    assert(index < getChildCount());
    auto childIter = children.begin() + index;
    for (auto iter = childIter + 1; iter != children.end(); ++iter)
        (*iter)->position = (*iter)->position - 1;

    children.erase(childIter);
}

void RefPropertyItem::removeChildren()
{
    children.clear();
}

void RefPropertyItem::addObject(const PropertyNode* node)
{
    nodes.push_back(node);
}

void RefPropertyItem::removeObject(const PropertyNode* object)
{
    auto iter = std::find(nodes.begin(), nodes.end(), object);
    if (iter == nodes.end())
        return;

    nodes.erase(iter);
}

////////////////////////////////////////////////////////////////////////////////////////////////

ReflectedPropertyModel::ReflectedPropertyModel(IDefinitionManager & definitionManager_,
    ICommandManager & commandManager_)
    : definitionManager(definitionManager_)
    , commandManager(commandManager_)
    , getterExtension(GetterExtension::createDummy())
    , setterExtension(SetterExtension::createDummy())
    , childCreator(definitionManager_)
{
    rootItem.reset(new RefPropertyItem(*this));

    using namespace std::placeholders;
    childCreator.nodePositionChanged.connect(std::bind(&ReflectedPropertyModel::childPositionChanged, this, _1, _2));
    childCreator.nodeCreated.connect(std::bind(&ReflectedPropertyModel::childAdded, this, _1, _2, _3));
    childCreator.nodeRemoved.connect(std::bind(&ReflectedPropertyModel::childRemoved, this, _1));

    registerExtension(new DefaultGetterExtension());
    registerExtension(new UrlGetterExtension());
    registerExtension(new DefaultChildCheatorExtension());
}

ReflectedPropertyModel::~ReflectedPropertyModel()
{
    rootItem.reset();
    GetterExtension::deleteExtensionChain(getterExtension);
    SetterExtension::deleteExtensionChain(setterExtension);
}

void ReflectedPropertyModel::update()
{
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    update(rootItem.get());
    double duration = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count();
    NGT_TRACE_MSG("update duration : %f seconds\n", duration);
}

void ReflectedPropertyModel::update(RefPropertyItem* item)
{
    for (const PropertyNode* node : item->nodes)
    {
        childCreator.updateSubTree(node);
    }

    for (const std::unique_ptr<RefPropertyItem>& child : item->children)
    {
        update(child.get());
    }

    Variant value = item->evalValue(definitionManager);
    if (value != item->itemValue)
    {
        notifyPreDataChanged(item, 1, ValueRole::roleId_, item->itemValue);
        item->itemValue = value;
        notifyPostDataChanged(item, 1, ValueRole::roleId_, item->itemValue);
    }
}

void ReflectedPropertyModel::setObjects(const std::vector<ObjectHandle>& objects)
{
    size_t childCount = rootItem->getChildCount();
    if (childCount != 0)
    {
        nodeToItem.clear();
        notifyPreItemsRemoved(nullptr, 0, childCount);
        rootItem->removeChildren();
        notifyPostItemsRemoved(nullptr, 0, childCount);
        rootItem->nodes.clear();
        childCreator.clear();
    }

    for (const ObjectHandle& handle : objects)
    {
        const PropertyNode* rootNode = childCreator.createRoot(handle);
        nodeToItem.emplace(rootNode, rootItem.get());
        rootItem->addObject(rootNode);
    }

    update();
}

void ReflectedPropertyModel::addObjects(const std::vector<ObjectHandle>& object)
{
}

void ReflectedPropertyModel::removeObjects(const std::vector<ObjectHandle>& object)
{
}

IItem * ReflectedPropertyModel::item(size_t index, const IItem * parent) const
{
    return getEffectiveParent(parent)->getChild(index);
}

ITreeModel::ItemIndex ReflectedPropertyModel::index(const IItem * item) const
{
    assert(item != nullptr);
    const RefPropertyItem * refItem = static_cast<const RefPropertyItem *>(item);
    return ITreeModel::ItemIndex(refItem->getPosition(), getModelParent(refItem->getParent()));
}

size_t ReflectedPropertyModel::size(const IItem * item) const
{
    return getEffectiveParent(item)->getChildCount();
}

int ReflectedPropertyModel::columnCount() const
{
    return 2;
}

void ReflectedPropertyModel::registerExtension(GetterExtension* extension)
{
    getterExtension = getterExtension->addExtension(extension);
}

void ReflectedPropertyModel::unregisterExtension(GetterExtension* extension)
{
    getterExtension = getterExtension->removeExtension(extension);
}

void ReflectedPropertyModel::registerExtension(SetterExtension * extension)
{
    setterExtension = setterExtension->addExtension(extension);
}

void ReflectedPropertyModel::unregisterExtension(SetterExtension * extension)
{
    setterExtension = setterExtension->removeExtension(extension);
}

void ReflectedPropertyModel::registerExtension(ChildCreatorExtension * extension)
{
    childCreator.registerExtension(extension);
}

void ReflectedPropertyModel::unregisterExtension(ChildCreatorExtension * extension)
{
    childCreator.unregisterExtension(extension);
}

void ReflectedPropertyModel::childPositionChanged(const PropertyNode* node, size_t childPosition)
{
}

void ReflectedPropertyModel::childAdded(const PropertyNode* parent, const PropertyNode* node, size_t childPosition)
{
    auto iter = nodeToItem.find(parent);
    assert(iter != nodeToItem.end());

    RefPropertyItem* item = iter->second;
    size_t childCount = item->getChildCount();
    const IItem* modelParent = getModelParent(item);

    notifyPreItemsInserted(modelParent, childCount, 1);
    RefPropertyItem * child = item->createChild();
    child->addObject(node);
    auto newNode = nodeToItem.emplace(node, child);
    assert(newNode.second);
    notifyPostItemsInserted(modelParent, childCount, 1);
}

void ReflectedPropertyModel::childRemoved(const PropertyNode* node)
{
    auto iter = nodeToItem.find(node);
    assert(iter != nodeToItem.end());
    
    RefPropertyItem* item = iter->second;
    RefPropertyItem* parent = item->getParent();
    size_t positionIndex = item->getPosition();
    const IItem* modelParent = getModelParent(parent);

    notifyPreItemsRemoved(modelParent, positionIndex, 1);
    parent->removeChild(positionIndex);
    nodeToItem.erase(node);
    notifyPostItemsRemoved(modelParent, positionIndex, 1);
}

const RefPropertyItem* ReflectedPropertyModel::getEffectiveParent(const IItem* modelParent) const
{
    return getEffectiveParent(const_cast<IItem *>(modelParent));
}

RefPropertyItem * ReflectedPropertyModel::getEffectiveParent(IItem * modelParent) const
{
    return modelParent == nullptr ? rootItem.get() : static_cast<RefPropertyItem *>(modelParent);
}

const IItem* ReflectedPropertyModel::getModelParent(const RefPropertyItem* effectiveParent) const
{
    return getModelParent(const_cast<RefPropertyItem *>(effectiveParent));
}

IItem * ReflectedPropertyModel::getModelParent(RefPropertyItem * effectiveParent) const
{
    return effectiveParent == rootItem.get() ? nullptr : effectiveParent;
}

Variant ReflectedPropertyModel::getData(const RefPropertyItem * item, int column, size_t roleId) const
{
    return getterExtension->getValue(item, column, roleId, definitionManager);
}

bool ReflectedPropertyModel::setData(RefPropertyItem * item, int column, size_t roleId, const Variant & data)
{
    return setterExtension->setValue(item, column, roleId, data, definitionManager, commandManager);
}
