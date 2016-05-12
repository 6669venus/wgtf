#include "default_property_model_extensions.hpp"
#include "reflected_collection_item.hpp"
#include "class_definition_model.hpp"
#include "reflected_enum_model.hpp"

#include "core_data_model/i_item_role.hpp"
#include "core_data_model/reflection/reflected_property_model.hpp"

#include "core_reflection/base_property.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"

#include "core_logging/logging.hpp"

namespace DPMEDetails
{
struct MaxMinValuePair
{
    MaxMinValuePair(const Variant & min, const Variant & max)
        : minValue_(min)
        , maxValue_(max)
    {
    }

    Variant minValue_;
    Variant maxValue_;
};

typedef std::unordered_map<const TypeId, MaxMinValuePair> MaxMinValuePairMap;

MaxMinValuePair getValuePair(const TypeId& tid)
{
    static const TypeId int8Type = TypeId::getType<int8_t>();
    static const TypeId int16Type = TypeId::getType<int16_t>();
    static const TypeId int32Type = TypeId::getType<int32_t>();
    static const TypeId int64Type = TypeId::getType<int64_t>();
    static const TypeId uint8Type = TypeId::getType<uint8_t>();
    static const TypeId uint16Type = TypeId::getType<uint16_t>();
    static const TypeId uint32Type = TypeId::getType<uint32_t>();
    static const TypeId uint64Type = TypeId::getType<uint64_t>();
    static const TypeId longType = TypeId::getType<long>();
    static const TypeId ulongType = TypeId::getType<unsigned long>();
    static const TypeId floatType = TypeId::getType<float>();
    static const TypeId doubleType = TypeId::getType<double>();

    if (int8Type == tid)
        return MaxMinValuePair(std::numeric_limits<int8_t>::lowest(), std::numeric_limits<int8_t>::max());

    if (int16Type == tid)
        return MaxMinValuePair(std::numeric_limits<int16_t>::lowest(), std::numeric_limits<int16_t>::max());

    if (int32Type == tid)
        return MaxMinValuePair(std::numeric_limits<int32_t>::lowest(), std::numeric_limits<int32_t>::max());

    if (int64Type == tid)
        return MaxMinValuePair(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());

    if (uint8Type == tid)
        return MaxMinValuePair(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());

    if (uint16Type == tid)
        return MaxMinValuePair(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());

    if (uint32Type == tid)
        return MaxMinValuePair(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint32_t>::max());

    if (uint64Type == tid)
        return MaxMinValuePair(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint64_t>::max());

    if (longType == tid)
        return MaxMinValuePair(std::numeric_limits<long>::lowest(), std::numeric_limits<long>::max());

    if (ulongType == tid)
        return MaxMinValuePair(std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max());

    if (floatType == tid)
        return MaxMinValuePair(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());

    if (doubleType == tid)
        return MaxMinValuePair(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

    return MaxMinValuePair(Variant(), Variant());
}

Variant getMaxValue(const TypeId & typeId)
{
    return getValuePair(typeId).maxValue_;
}

Variant getMinValue(const TypeId & typeId)
{
    return getValuePair(typeId).minValue_;
}
}

Variant DefaultGetterExtension::getValue(const RefPropertyItem* item, int column, size_t roleId, IDefinitionManager& defMng) const
{
    if (roleId == ValueTypeRole::roleId_)
    {
        return item->getProperty()->getType().getName();
    }
    else if (roleId == ValueRole::roleId_)
    {
        return item->getValue();
    }
    else if (roleId == IndexPathRole::roleId_)
    {
        return item->getIndexPath();
    }
    else if (roleId == IsEnumRole::roleId_)
    {
        return findFirstMetaData<MetaEnumObj>(*item->getProperty(), defMng) != nullptr;
    }
    else if (roleId == EnumModelRole::roleId_)
    {
        const MetaEnumObj* enumObj = findFirstMetaData<MetaEnumObj>(*item->getProperty(), defMng);
        if (enumObj)
        {
            auto enumModel = std::unique_ptr< IListModel >(new ReflectedEnumModel(enumObj));
            return ObjectHandle(std::move(enumModel));
        }
    }
    else if (roleId == IsThumbnailRole::roleId_)
    {
        return findFirstMetaData<MetaThumbnailObj>(*item->getProperty(), defMng) != nullptr;
    }
    else if (roleId == IsSliderRole::roleId_)
    {
        return findFirstMetaData<MetaSliderObj>(*item->getProperty(), defMng) != nullptr;
    }
    else if (roleId == IsColorRole::roleId_)
    {
        return findFirstMetaData<MetaColorObj>(*item->getProperty(), defMng) != nullptr;
    }
    else if (roleId == MinValueRole::roleId_)
    {
        IBasePropertyPtr propertyInstance = item->getProperty();
        TypeId typeId = propertyInstance->getType();
        Variant defaultMinValue = DPMEDetails::getMinValue(typeId);
        auto minMaxObj = findFirstMetaData<MetaMinMaxObj>(*propertyInstance, defMng);
        if (minMaxObj != nullptr)
        {
            const float & value = minMaxObj->getMin();
            float minValue = .0f;
            bool isOk = defaultMinValue.tryCast(minValue);
            assert(isOk);
            float diff = minValue - value;
            float epsilon = std::numeric_limits<float>::epsilon();
            if (diff > epsilon)
            {
                NGT_ERROR_MSG("Property %s: MetaMinMaxObj min value exceeded limits.\n", propertyInstance->getName());
            }
            else
                return value;
        }

        return defaultMinValue;
    }
    else if (roleId == MaxValueRole::roleId_)
    {
        IBasePropertyPtr propertyInstance = item->getProperty();
        TypeId typeId = propertyInstance->getType();
        Variant defaultMinValue = DPMEDetails::getMaxValue(typeId);
        auto minMaxObj = findFirstMetaData<MetaMinMaxObj>(*propertyInstance, defMng);
        if (minMaxObj != nullptr)
        {
            const float & value = minMaxObj->getMax();
            float minValue = .0f;
            bool isOk = defaultMinValue.tryCast(minValue);
            assert(isOk);
            float diff = value - minValue;
            float epsilon = std::numeric_limits<float>::epsilon();
            if (diff > epsilon)
            {
                NGT_ERROR_MSG("Property %s: MetaMinMaxObj max value exceeded limits.\n", propertyInstance->getName());
            }
            else
                return value;
        }

        return defaultMinValue;
    }
    else if (roleId == StepSizeRole::roleId_)
    {
        IBasePropertyPtr propertyInstance = item->getProperty();
        TypeId typeId = propertyInstance->getType();
        auto stepSize = findFirstMetaData<MetaStepSizeObj>(*propertyInstance, defMng);
        if (stepSize != nullptr)
            return stepSize->getStepSize();

        return MetaStepSizeObj::DefaultStepSize;
    }
    else if (roleId == DecimalsRole::roleId_)
    {
        IBasePropertyPtr propertyInstance = item->getProperty();
        TypeId typeId = propertyInstance->getType();
        auto decimals = findFirstMetaData< MetaDecimalsObj >(*propertyInstance, defMng);
        if (decimals != nullptr)
            return decimals->getDecimals();

        return MetaDecimalsObj::DefaultDecimals;
    }
    else if (roleId == DefinitionRole::roleId_)
    {
        IBasePropertyPtr propertyInstance = item->getProperty();
        if (!propertyInstance->isValue())
            return Variant();

        Variant value = item->getValue();
        ObjectHandle provider;
        if (!value.tryCast(provider))
            return Variant();

        provider = reflectedRoot(provider, defMng);
        IClassDefinition * definition = const_cast<IClassDefinition *>(provider.isValid() ? provider.getDefinition(defMng)
                                                                                          : nullptr);
        return ObjectHandle(definition);
    }
    else if (roleId == DefinitionModelRole::roleId_)
    {
        IBasePropertyPtr propertyInstance = item->getProperty();
        TypeId typeId = propertyInstance->getType();
        if (typeId.isPointer())
        {
            auto definition = defMng.getDefinition(typeId.removePointer().getName());
            if (definition != nullptr)
            {
                auto definitionModel = std::unique_ptr<IListModel>(new ClassDefinitionModel(definition, defMng));
                return ObjectHandle(std::move(definitionModel));
            }
        }
    }

    return GetterExtension::getValue(item, column, roleId, defMng);
}

Variant UrlGetterExtension::getValue(const RefPropertyItem* item, int column, size_t roleId, IDefinitionManager & defMng) const
{
    if (roleId == IsUrlRole::roleId_)
    {
        return findFirstMetaData<MetaUrlObj>(*item->getProperty(), defMng) != nullptr;
    }
    else if (roleId == UrlIsAssetBrowserRole::roleId_)
    {
        bool isAssetBrowserDlg = false;
        const MetaUrlObj* urlObj = findFirstMetaData<MetaUrlObj>(*item->getProperty(), defMng);
        if (urlObj != nullptr)
        {
            isAssetBrowserDlg = urlObj->isAssetBrowserDialog();
        }
        return isAssetBrowserDlg;
    }
    else if (roleId == UrlDialogTitleRole::roleId_)
    {
        const char * title = nullptr;
        const MetaUrlObj* urlObj = findFirstMetaData<MetaUrlObj>(*item->getProperty(), defMng);
        if (urlObj != nullptr)
        {
            title = urlObj->getDialogTitle();
        }
        return title;
    }
    else if (roleId == UrlDialogDefaultFolderRole::roleId_)
    {
        const char * folder = nullptr;
        const MetaUrlObj* urlObj = findFirstMetaData<MetaUrlObj>(*item->getProperty(), defMng);
        if (urlObj != nullptr)
        {
            folder = urlObj->getDialogDefaultFolder();
        }
        return folder;
    }
    else if (roleId == UrlDialogNameFiltersRole::roleId_)
    {
        const char * nameFilters = nullptr;
        const MetaUrlObj* urlObj = findFirstMetaData<MetaUrlObj>(*item->getProperty(), defMng);
        if (urlObj != nullptr)
        {
            nameFilters = urlObj->getDialogNameFilters();
        }
        return nameFilters;
    }
    else if (roleId == UrlDialogSelectedNameFilterRole::roleId_)
    {
        const char * selectedFilter = nullptr;
        const MetaUrlObj* urlObj = findFirstMetaData<MetaUrlObj>(*item->getProperty(), defMng);
        if (urlObj != nullptr)
        {
            selectedFilter = urlObj->getDialogSelectedNameFilter();
        }
        return selectedFilter;
    }
    else if (roleId == UrlDialogModalityRole::roleId_)
    {
        int modality = 1;
        const MetaUrlObj* urlObj = findFirstMetaData<MetaUrlObj>(*item->getProperty(), defMng);
        if (urlObj != nullptr)
        {
            const int & value = urlObj->getDialogModality();
            if (value >= 0 && value <= 2)
            {
                modality = value;
            }
        }
        return modality;
    }

    return GetterExtension::getValue(item, column, roleId, defMng);
}

void DefaultChildCheatorExtension::exposeChildren(const PropertyNode& node, std::vector<const PropertyNode*>& children, IDefinitionManager& defMng) const
{
    if (node.propertyType == PropertyNode::RealProperty || node.propertyType == PropertyNode::CollectionItem)
    {
        Variant propertyValue = node.propertyInstance->get(node.object, defMng);

        Collection collectionHandle;
        ObjectHandle handle;
        if (propertyValue.tryCast(handle))
        {
            const IClassDefinition* definition = handle.getDefinition(defMng);
            if (definition)
            {
                PropertyIteratorRange range = definition->allProperties();
                for (PropertyIterator iter = range.begin(); iter != range.end(); ++iter)
                {
                    children.push_back(allocator->createPropertyNode(iter.get(), handle));
                }
            }
        }
        else if (propertyValue.tryCast(collectionHandle))
        {
            for (Collection::Iterator iter = collectionHandle.begin(); iter != collectionHandle.end(); ++iter)
            {
                std::string name = BuildIteratorPropertyName(iter);
                TypeId valueType = iter.valueType();
                IBasePropertyPtr propertyInstance = allocator->getCollectionItemProperty(std::move(name), std::move(valueType), defMng);
                ReflectedIteratorValue iteratorValue{ iter, iter.value() };
                children.push_back(allocator->createPropertyNode(propertyInstance, ObjectHandle(iteratorValue), PropertyNode::CollectionItem));
            }
        }
    }

    return ChildCreatorExtension::exposeChildren(node, children, defMng);
}

class DefaultAllocator: public IChildAllocator
{
public:
    ~DefaultAllocator();
    const PropertyNode* createPropertyNode(IBasePropertyPtr propertyInstance, ObjectHandle object, int type = PropertyNode::RealProperty) override;
    void deletePropertyNode(const PropertyNode* node) override;
    IBasePropertyPtr getCollectionItemProperty(std::string&& name, const TypeId& type, IDefinitionManager & defMng) override;

private:
    std::set<const PropertyNode*> allocatedNodes;

    struct EmptyDeleter
        : public std::unary_function<ReflectedIteratorProperty *, void>
    {
        void operator()(ReflectedIteratorProperty *){}
    };

    struct PropertyHash
        : public std::unary_function<const std::shared_ptr<ReflectedIteratorProperty>, uint64_t>
    {
        uint64_t operator()(const std::shared_ptr<ReflectedIteratorProperty> & v) const
        {
            uint64_t seed = 0;
            HashUtilities::directCombine(seed, v->getNameHash());
            HashUtilities::directCombine(seed, v->getType().getHashcode());
            return seed;
        }
    };

    struct PropertyEqual :
        public std::binary_function<const std::shared_ptr<ReflectedIteratorProperty>,
                                    const std::shared_ptr<ReflectedIteratorProperty>, bool>
    {
        bool operator()(const std::shared_ptr<ReflectedIteratorProperty> & v1, const std::shared_ptr<ReflectedIteratorProperty> & v2) const
        {
            return strcmp(v1->getName(), v2->getName()) == 0 &&
                v1->getType() == v2->getType();
        }
    };

    std::unordered_set<std::shared_ptr<ReflectedIteratorProperty>, PropertyHash, PropertyEqual> propertiesPool;
};

DefaultAllocator::~DefaultAllocator()
{
    assert(allocatedNodes.empty());
}

const PropertyNode* DefaultAllocator::createPropertyNode(IBasePropertyPtr propertyInstance, ObjectHandle object, int type /*= PropertyNode::RealProperty*/)
{
    auto insertPair = allocatedNodes.insert(new PropertyNode(type, propertyInstance, object));
    assert(insertPair.second == true);

    return *insertPair.first;
}

void DefaultAllocator::deletePropertyNode(const PropertyNode* node)
{
    allocatedNodes.erase(node);
    delete node;
}

IBasePropertyPtr DefaultAllocator::getCollectionItemProperty(std::string&& name, const TypeId& type, IDefinitionManager & defMng)
{
    ReflectedIteratorProperty etalonItem(std::move(name), type, defMng);
    std::shared_ptr<ReflectedIteratorProperty> etalonPointer(&etalonItem, EmptyDeleter());

    auto iter = propertiesPool.find(etalonPointer);
    if (iter != propertiesPool.end())
        return *iter;

    auto emplacePair = propertiesPool.emplace(new ReflectedIteratorProperty(std::string(etalonItem.getName()), type, defMng));
    assert(emplacePair.second == true);
    return *emplacePair.first;
}

std::shared_ptr<IChildAllocator> createDefaultAllocator()
{
    return std::make_shared<DefaultAllocator>();
}

RefPropertyItem * DefaultMergeValueExtension::lookUpItem(const PropertyNode* node, const std::vector<std::unique_ptr<RefPropertyItem>>& items,
                                                         IDefinitionManager & definitionManager) const
{
    RefPropertyItem* result = nullptr;
    Variant nodeValue = node->propertyInstance->get(node->object, definitionManager);
    ObjectHandle nodeHandle;
    nodeValue.tryCast(nodeHandle);
    for (const std::unique_ptr<RefPropertyItem>& item : items)
    {
        IBasePropertyPtr itemProperty = item->getProperty();
        if (node->propertyInstance == itemProperty)
        {
            Variant itemValue = itemProperty->get(item->getObjects().front()->object, definitionManager);
            ObjectHandle itemHandle;
            if (nodeHandle.isValid() && itemValue.tryCast(itemHandle))
            {
                if (nodeHandle.type() == itemHandle.type())
                {
                    result = item.get();
                    break;
                }
            }
            else if (nodeValue.type() == itemValue.type())
            {
                result = item.get();
                break;
            }
        }
    }

    return result;
}
