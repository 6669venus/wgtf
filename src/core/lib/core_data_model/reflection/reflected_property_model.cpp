#include "reflected_property_model.hpp"

#include "core_data_model/reflection/default_property_model_extensions.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/base_property.hpp"

#include <set>

namespace RPMDetails
{
const int ValueColumn = 1;
}


RefPropertyItem::RefPropertyItem(ReflectedPropertyModel & model_)
    : model(model_)
    , parent(nullptr)
    , position(0)
{
}

RefPropertyItem::RefPropertyItem(RefPropertyItem * parent_, size_t position_)
    : model(parent_->model)
    , parent(parent_)
    , position(position_)
{
}

void RefPropertyItem::injectData(size_t roleId, const Variant& value)
{
    injectedData[roleId] = value;
}

Variant RefPropertyItem::getInjectedData(size_t roleId)
{
    auto iter = injectedData.find(roleId);
    if (iter != injectedData.end())
    {
        return iter->second;
    }

    return Variant();
}

const RefPropertyItem * RefPropertyItem::getParent() const
{
    return parent;
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
    if (column == RPMDetails::ValueColumn)
    {
        auto iter = injectedData.find(roleId);
        if (iter != injectedData.end())
        {
            return iter->second;
        }
    }

    return model.getData(this, column, roleId);
}

bool RefPropertyItem::setData(int column, size_t roleId, const Variant & data)
{
    return false;
    // TODO uncomment this after back write on multiselection will be fixed
    //return model.setData(this, column, roleId, data);
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

            item = static_cast<const RefPropertyItem*>(item->getNonConstParent());
        }

        indexPath.shrink_to_fit();
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

        std::shared_ptr<const PropertyNode> node = *iter;
        result = node->propertyInstance->get(node->object, definitionManager);
        ++iter;
        for (iter; iter != nodes.end(); ++iter)
        {
            std::shared_ptr<const PropertyNode> node = *iter;
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

void RefPropertyItem::setValue(Variant && newValue)
{
    itemValue = std::move(newValue);
}

RefPropertyItem* RefPropertyItem::getNonConstParent() const
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

void RefPropertyItem::addObject(const std::shared_ptr<const PropertyNode>& node)
{
#ifdef _DEBUG
    if (!nodes.empty())
    {
        assert(nodes.front()->propertyInstance == node->propertyInstance);
    }
#endif

    nodes.push_back(node);
}

void RefPropertyItem::removeObject(const std::shared_ptr<const PropertyNode>& object)
{
    auto iter = std::find(nodes.begin(), nodes.end(), object);
    if (iter == nodes.end())
        return;

    nodes.erase(iter);
}

void RefPropertyItem::removeObjects()
{
    nodes.clear();
}

bool RefPropertyItem::hasObjects() const
{
    return !nodes.empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////

ReflectedPropertyModel::ReflectedPropertyModel(IDefinitionManager & definitionManager_,
    ICommandManager & commandManager_, IReflectionController& refController)
    : definitionManager(definitionManager_)
    , commandManager(commandManager_)
    , getterExtension(GetterExtension::createDummy())
    , setterExtension(SetterExtension::createDummy())
    , mergeExtension(MergeValuesExtension::createDummy())
    , injectExtension(InjectDataExtension::createDummy())
    , childCreator(definitionManager_)
{
    rootItem.reset(new RefPropertyItem(*this));

    using namespace std::placeholders;
    childCreator.nodeCreated.connect(std::bind(&ReflectedPropertyModel::childAdded, this, _1, _2, _3));
    childCreator.nodeRemoved.connect(std::bind(&ReflectedPropertyModel::childRemoved, this, _1));

    registerExtension(new DefaultGetterExtension());
    registerExtension(new DefaultSetterExtension(refController));
    registerExtension(new UrlGetterExtension());
    registerExtension(new DefaultChildCheatorExtension());
    registerExtension(new DefaultMergeValueExtension());
}

ReflectedPropertyModel::~ReflectedPropertyModel()
{
    rootItem.reset();
    GetterExtension::deleteExtensionChain(getterExtension);
    SetterExtension::deleteExtensionChain(setterExtension);
    MergeValuesExtension::deleteExtensionChain(mergeExtension);
    InjectDataExtension::deleteExtensionChain(injectExtension);
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
    for (const std::shared_ptr<const PropertyNode> & node : item->nodes)
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
        item->setValue(std::move(value));
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
        rootItem->removeObjects();
        childCreator.clear();
    }

    for (const ObjectHandle& handle : objects)
    {
        std::shared_ptr<const PropertyNode> rootNode = childCreator.createRoot(handle);
        nodeToItem.emplace(rootNode, rootItem.get());
        rootItem->addObject(rootNode);
    }

    update();
}

IItem * ReflectedPropertyModel::item(size_t index, const IItem * parent) const
{
    return getEffectiveParent(parent)->getChild(index);
}

ITreeModel::ItemIndex ReflectedPropertyModel::index(const IItem * item) const
{
    assert(item != nullptr);
    const RefPropertyItem * refItem = static_cast<const RefPropertyItem *>(item);
    return ITreeModel::ItemIndex(refItem->getPosition(), getModelParent(refItem->getNonConstParent()));
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

void ReflectedPropertyModel::registerExtension(MergeValuesExtension * extension)
{
    mergeExtension = mergeExtension->addExtension(extension);
}

void ReflectedPropertyModel::unregisterExtension(MergeValuesExtension * extension)
{
    mergeExtension = mergeExtension->removeExtension(extension);
}

void ReflectedPropertyModel::registerExtension(ChildCreatorExtension * extension)
{
    childCreator.registerExtension(extension);
}

void ReflectedPropertyModel::unregisterExtension(ChildCreatorExtension * extension)
{
    childCreator.unregisterExtension(extension);
}

void ReflectedPropertyModel::registerExtension(InjectDataExtension* extension)
{
    injectExtension = injectExtension->addExtension(extension);
}

void ReflectedPropertyModel::unregisterExtension(InjectDataExtension* extension)
{
    injectExtension = injectExtension->removeExtension(extension);
}

void ReflectedPropertyModel::childAdded(const std::shared_ptr<const PropertyNode>& parent, const std::shared_ptr<const PropertyNode>& node, size_t childPosition)
{
    auto iter = nodeToItem.find(parent);
    assert(iter != nodeToItem.end());

    RefPropertyItem* parentItem = iter->second;

    RefPropertyItem* childItem = mergeExtension->lookUpItem(node, parentItem->children, definitionManager);
    if (childItem != nullptr)
    {
        childItem->addObject(node);
        injectExtension->updateInjection(childItem);
    }
    else
    {
        size_t childCount = parentItem->getChildCount();
        const IItem* modelParent = getModelParent(parentItem);

        notifyPreItemsInserted(modelParent, childCount, 1);
        childItem = parentItem->createChild();
        childItem->addObject(node);
        notifyPostItemsInserted(modelParent, childCount, 1);
        using namespace std::placeholders;
        injectExtension->inject(childItem);
    }

    auto newNode = nodeToItem.emplace(node, childItem);
    assert(newNode.second);
}

void ReflectedPropertyModel::childRemoved(const std::shared_ptr<const PropertyNode>& node)
{
    auto iter = nodeToItem.find(node);
    assert(iter != nodeToItem.end());
    
    RefPropertyItem* item = iter->second;
    item->removeObject(node);
    nodeToItem.erase(node);

    if (!item->hasObjects())
    {
        RefPropertyItem* parent = item->getNonConstParent();
        size_t positionIndex = item->getPosition();
        const IItem* modelParent = getModelParent(parent);
        notifyPreItemsRemoved(modelParent, positionIndex, 1);
        parent->removeChild(positionIndex);
        notifyPostItemsRemoved(modelParent, positionIndex, 1);
    }
    else
    {
        injectExtension->updateInjection(item);
    }
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
