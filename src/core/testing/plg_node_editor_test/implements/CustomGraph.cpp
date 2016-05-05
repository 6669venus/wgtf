
#include "CustomGraph.h"
#include "CustomNode.h"
#include "CustomConnection.h"

#include "core_logging\logging.hpp"

std::shared_ptr<INode> CustomGraph::CreateNode(std::string nodeClass, float x, float y)
{  
    std::shared_ptr<INode> node(new CustomNode(nodeClass));
    node->SetPos(x, y);
    m_nodesModel.push_back(node);
    
    return node;
}

void CustomGraph::DeleteNode(size_t nodeID)
{
    auto nodeIter = GetNodeIterById(nodeID);
    if (nodeIter == m_nodesModel.end())
    {
        NGT_ERROR_MSG("Failed to delete node with ID: %d", nodeID);
        return;
    }        
    
    ObjectHandleT<INode> deleteNode = *nodeIter;
    std::vector<size_t> deleteConnections;
    deleteConnections.reserve(m_connectionsModel.size());

    for (auto connection : m_connectionsModel)
    {
        size_t inputSlotID = connection->Input()->Id();
        size_t outputSlotID = connection->Output()->Id();

        if (deleteNode->GetSlotById(inputSlotID) != nullptr ||
            deleteNode->GetSlotById(outputSlotID) != nullptr)
        {
            deleteConnections.push_back(connection->Id());
        }
    }

    for (auto connId : deleteConnections)
    {
        DeleteConnection(connId);
    }
    m_nodesModel.erase(nodeIter);
}

GenericListT<ObjectHandleT<INode>>::Iterator CustomGraph::GetNodeIterById(size_t nodeId)
{
    auto nodePos = std::find_if(m_nodesModel.begin(), m_nodesModel.end(), [nodeId](const ObjectHandleT<INode> &node) {
        return nodeId == node->Id();
    });

    return nodePos;
}

ObjectHandleT<IConnection> CustomGraph::CreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo)
{
    ObjectHandleT<INode> nodeFrom;
    ObjectHandleT<ISlot> slotFrom;
    ObjectHandleT<INode> nodeTo;
    ObjectHandleT<ISlot> slotTo;

    bool result = true;
    while (true)
    {
        auto nodeIterFrom = GetNodeIterById(nodeIdFrom);
        if (nodeIterFrom == m_nodesModel.end())
        {
            result = false;
            break;
        }
        nodeFrom = *nodeIterFrom;

        slotFrom = nodeFrom->GetSlotById(slotIdFrom);
        if (slotFrom == nullptr)
        {
            result = false;
            break;
        }

        auto nodeIterTo = GetNodeIterById(nodeIdTo);
        if (nodeIterTo == m_nodesModel.end())
        {
            result = false;
            break;
        }
        nodeTo = *nodeIterTo;

        slotTo = nodeTo->GetSlotById(slotIdTo);
        if (slotTo == nullptr)
        {
            result = false;
        }

        break;
    }

    if (!result)
    {
        return nullptr;
    }

    ObjectHandleT<IConnection> connection(new CustomConnection());    
    result = connection->Bind(slotFrom, slotTo);

    if (result)
    {
        m_connectionsModel.push_back(connection);
        return connection;
    }

    return nullptr;
}

void CustomGraph::DeleteConnection(size_t connectionId)
{
    auto connectionPos = std::find_if(m_connectionsModel.begin(), m_connectionsModel.end(), [connectionId](ObjectHandleT<IConnection> connection) {
        return connectionId == connection->Id();
    });

    if (connectionPos != m_connectionsModel.end())
    {
        m_connectionsModel.erase(connectionPos);
    }
}

bool CustomGraph::Validate(std::string& errorMessage)
{
    //TODO: Need implementation
    return true;
}

void CustomGraph::Save(std::string fileName)
{
    //TODO: Need implementation
}

void CustomGraph::Load(std::string fileName)
{
    //TODO: Need implementation
}