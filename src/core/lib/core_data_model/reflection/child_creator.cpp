#include "child_creator.hpp"
#include "default_property_model_extensions.hpp"
#include "reflected_collection_item.hpp"

namespace CCDetails
{

bool IsPropertyEqual(const PropertyNode* left, const PropertyNode* right)
{
    assert(left != nullptr);
    assert(right != nullptr);
    if (left == right)
    {
        return true;
    }

    return left->propertyType == right->propertyType && left->propertyInstance == right->propertyInstance;
}

struct PropertyNodeEqual
{
    bool operator()(const PropertyNode* left, const PropertyNode* right) const
    {
        assert(left != nullptr);
        assert(right != nullptr);
        if (left == right)
        {
            return true;
        }

        bool isEqualProperty = left->propertyType == right->propertyType &&
                               left->propertyInstance == right->propertyInstance;

        if (!isEqualProperty)
            return false;

        if (left->propertyType == PropertyNode::CollectionItem)
        {
            ReflectedIteratorValue * leftIter = left->object.getBase<ReflectedIteratorValue>();
            ReflectedIteratorValue * rightIter = right->object.getBase<ReflectedIteratorValue>();
            assert(leftIter != nullptr);
            assert(rightIter != nullptr);

            return leftIter->value == rightIter->value;
        }

        return left->object == right->object;
    }
};

struct PropertyNodeLess
{
    bool operator()(const PropertyNode* left, const PropertyNode* right) const
    {
        assert(left != nullptr);
        assert(right != nullptr);

        if (left == right)
        {
            return false;
        }

        if (left->propertyType != right->propertyType)
            return left->propertyType < right->propertyType;

        if (left->propertyInstance != right->propertyInstance)
            return left->propertyInstance < right->propertyInstance;

        PropertyNodeEqual equal;
        if (equal(left, right))
            return false;

        return left->object < right->object;
    }
};

}

ChildCreator::ChildCreator(IDefinitionManager& defMng)
    : definitionManager(defMng)
    , allocator(createDefaultAllocator())
{
}

ChildCreator::~ChildCreator()
{
    assert(propertiesIndex.empty());
    ChildCreatorExtension::deleteExtensionChain(extensions);
}

const PropertyNode* ChildCreator::createRoot(const ObjectHandle& handle)
{
    auto result = propertiesIndex.emplace(MakeRootNode(handle, *allocator), std::vector<const PropertyNode*>());
    assert(result.second == true);
    return result.first->first;
}

void ChildCreator::updateSubTree(const PropertyNode* parent)
{
    assert(parent != nullptr);
    std::vector<const PropertyNode*> children;
    extensions->exposeChildren(*parent, children, definitionManager);

    auto iter = propertiesIndex.find(parent);
    if (iter == propertiesIndex.end())
    {
        // insert new parent into index and store it's children
        std::vector<const PropertyNode*>& newItems = propertiesIndex[parent];
        newItems = std::move(children);
        for (size_t i = 0; i < newItems.size(); ++i)
        {
            nodeCreated(parent, newItems[i], i);
        }
    }
    else
    {
        CCDetails::PropertyNodeEqual equalCmp;

        bool childrenVectorsEqual = true;
        std::vector<const PropertyNode*>& currentChildren = iter->second;
        if (children.size() == currentChildren.size())
        {
            for (size_t i = 0; i < children.size(); ++i)
            {
                if (!equalCmp(children[i], currentChildren[i]))
                {
                    childrenVectorsEqual = false;
                    break;
                }
            }
        }
        else
        {
            childrenVectorsEqual = false;
        }

        if (childrenVectorsEqual)
        {
            using namespace std::placeholders;
            std::for_each(children.begin(), children.end(), std::bind(&IChildAllocator::deletePropertyNode, allocator.get(), _1));
            return;
        }

        for (auto iter = currentChildren.rbegin(); iter != currentChildren.rend(); ++iter)
        {
            removeNode(*iter);
        }

        std::swap(children, currentChildren);
        for (size_t i = 0; i < currentChildren.size(); ++i)
        {
            nodeCreated(parent, currentChildren[i], i);
        }
    }
}

void ChildCreator::removeNode(const PropertyNode* parent)
{
    auto iter = propertiesIndex.find(parent);
    assert(iter != propertiesIndex.end());
    std::vector<const PropertyNode *>& children = iter->second;
    for (const PropertyNode* node : children)
    {
        removeNode(node);
    }
    propertiesIndex.erase(iter);
    nodeRemoved(parent);
    allocator->deletePropertyNode(parent);
}

void ChildCreator::clear()
{
    std::set<const PropertyNode*> allNodes;
    std::unordered_map<const PropertyNode*, std::vector<const PropertyNode*>>::iterator iter = propertiesIndex.begin();
    for (iter; iter != propertiesIndex.end(); ++iter)
    {
        allNodes.insert(iter->first);
        allNodes.insert(iter->second.begin(), iter->second.end());
    }
    propertiesIndex.clear();

    using namespace std::placeholders;
    std::for_each(allNodes.begin(), allNodes.end(), std::bind(&IChildAllocator::deletePropertyNode, allocator.get(), _1));
}

void ChildCreator::registerExtension(ChildCreatorExtension* extension)
{
    extension->setAllocator(allocator);
    extensions = extensions->addExtension(extension);
}

void ChildCreator::unregisterExtension(ChildCreatorExtension* extension)
{
    extension->setAllocator(nullptr);
    extensions = extensions->removeExtension(extension);
}

