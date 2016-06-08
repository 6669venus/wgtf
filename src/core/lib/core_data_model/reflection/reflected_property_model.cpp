#include "reflected_property_model.hpp"

#include "core_data_model/reflection/default_property_model_extensions.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/base_property.hpp"

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

    return model.getDataImpl(this, column, roleId);
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

ReflectedPropertyModel::ReflectedPropertyModel(IComponentContext& context)
    : interfacesHolder(context)
    , childCreator(context)
{
    rootItem.reset(new RefPropertyItem(*this));

    registerExtension(SetterGetterExtension::createDummy());
    registerExtension(MergeValuesExtension::createDummy());
    registerExtension(InjectDataExtension::createDummy());

    using namespace std::placeholders;
    childCreator.nodeCreated.connect(std::bind(&ReflectedPropertyModel::childAdded, this, _1, _2, _3));
    childCreator.nodeRemoved.connect(std::bind(&ReflectedPropertyModel::childRemoved, this, _1));

    registerExtension(std::make_shared<DefaultSetterGetterExtension>(context));
    registerExtension(std::make_shared<UrlGetterExtension>());
    registerExtension(std::make_shared<DefaultChildCheatorExtension>());
    registerExtension(std::make_shared<DefaultMergeValueExtension>());
}

ReflectedPropertyModel::~ReflectedPropertyModel()
{
    rootItem.reset();
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
    INTERFACE_REQUEST(IDefinitionManager, defManager, interfacesHolder, void());

    for (const std::shared_ptr<const PropertyNode> & node : item->nodes)
    {
        childCreator.updateSubTree(node);
    }

    for (const std::unique_ptr<RefPropertyItem>& child : item->children)
    {
        update(child.get());
    }

    Variant value = item->evalValue(defManager);
    if (value != item->itemValue)
    {
        signalPreItemDataChanged(item, 1, ValueRole::roleId_, item->itemValue);
        item->setValue(std::move(value));
        signalPostItemDataChanged(item, 1, ValueRole::roleId_, item->itemValue);
    }
}

void ReflectedPropertyModel::setObjects(const std::vector<ObjectHandle>& objects)
{
    size_t childCount = rootItem->getChildCount();
    if (childCount != 0)
    {
        nodeToItem.clear();
        signalPreItemsRemoved(nullptr, 0, childCount);
        rootItem->removeChildren();
        signalPostItemsRemoved(nullptr, 0, childCount);
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

void ReflectedPropertyModel::childAdded(const std::shared_ptr<const PropertyNode>& parent, const std::shared_ptr<const PropertyNode>& node, size_t childPosition)
{
    INTERFACE_REQUEST(IDefinitionManager, defManager, interfacesHolder, void());

    auto iter = nodeToItem.find(parent);
    assert(iter != nodeToItem.end());

    RefPropertyItem* parentItem = iter->second;

    std::shared_ptr<InjectDataExtension> injectExtension = getExtensionChain<InjectDataExtension>();
    RefPropertyItem* childItem = getExtensionChain<MergeValuesExtension>()->lookUpItem(node, parentItem->children, defManager);
    if (childItem != nullptr)
    {
        childItem->addObject(node);
        injectExtension->updateInjection(childItem);
    }
    else
    {
        size_t childCount = parentItem->getChildCount();
        const IItem* modelParent = getModelParent(parentItem);

        signalPreItemsInserted(modelParent, childCount, 1);
        childItem = parentItem->createChild();
        childItem->addObject(node);
        signalPostItemsInserted(modelParent, childCount, 1);
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
        signalPreItemsRemoved(modelParent, positionIndex, 1);
        parent->removeChild(positionIndex);
        signalPostItemsRemoved(modelParent, positionIndex, 1);
    }
    else
    {
        getExtensionChain<InjectDataExtension>()->updateInjection(item);
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

Variant ReflectedPropertyModel::getDataImpl(const RefPropertyItem * item, int column, size_t roleId) const
{
    INTERFACE_REQUEST(IDefinitionManager, defManager, interfacesHolder, Variant());
    return getExtensionChain<SetterGetterExtension>()->getValue(item, column, roleId, defManager);
}

bool ReflectedPropertyModel::setDataImpl(RefPropertyItem * item, int column, size_t roleId, const Variant & data)
{
    INTERFACE_REQUEST(IDefinitionManager, defManager, interfacesHolder, false);
    INTERFACE_REQUEST(ICommandManager, commandManager, interfacesHolder, false);
    return getExtensionChain<SetterGetterExtension>()->setValue(item, column, roleId, data, defManager, commandManager);
}

void ReflectedPropertyModel::registerExtension(const std::shared_ptr<ExtensionChain>& extension)
{
    const TypeId& extType = extension->getType();
    if (extType == TypeId::getType<ChildCreatorExtension>())
    {
        childCreator.registerExtension(polymorphCast<ChildCreatorExtension>(extension));
        return;
    }

    auto iter = extensions.find(extType);
    if (iter == extensions.end())
    {
        extensions.emplace(extType, extension);
        return;
    }

    iter->second = ExtensionChain::addExtension(iter->second, extension);
}

void ReflectedPropertyModel::unregisterExtension(const std::shared_ptr<ExtensionChain>& extension)
{
    const TypeId& extType = extension->getType();
    if (extType == TypeId::getType<ChildCreatorExtension>())
    {
        childCreator.unregisterExtension(polymorphCast<ChildCreatorExtension>(extension));
        return;
    }

    auto iter = extensions.find(extType);
    if (iter == extensions.end())
    {
        /// you do something wrong
        assert(false);
        return;
    }

    iter->second = ExtensionChain::removeExtension(iter->second, extension);
}
