
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

void CustomGraph::DeleteNode(size_t nodeId)
{
    auto nodeIter = std::find_if(m_nodesModel.begin(), m_nodesModel.end(), [nodeId](const ObjectHandleT<INode> &node) {
        return nodeId == node->Id();
    });

    if (nodeIter == m_nodesModel.end())
    {
        NGT_ERROR_MSG("Failed to delete node with ID: %d", nodeId);
        return;
    }

    //TODO: Need refactoring
    //===============================================================================================
    ObjectHandleT<INode> node = *nodeIter;
    auto findConnectionBySlot = [&node](const ObjectHandleT<IConnection> &connection) {
        return (node->GetSlotById(connection->Input()->Id()) != nullptr ||
            node->GetSlotById(connection->Output()->Id()) != nullptr);
    };

    for (auto connectionPos = std::find_if(m_connectionsModel.begin(), m_connectionsModel.end(), findConnectionBySlot); connectionPos != m_connectionsModel.end();)
    {
        m_connectionsModel.erase(connectionPos);
        connectionPos = std::find_if(m_connectionsModel.begin(), m_connectionsModel.end(), findConnectionBySlot);
    }
    //===============================================================================================
    m_nodesModel.erase(nodeIter);
}

ObjectHandleT<IConnection> CustomGraph::CreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo)
{
    ObjectHandleT<INode> nodeFrom;
    ObjectHandleT<ISlot> slotFrom;
    ObjectHandleT<INode> nodeTo;
    ObjectHandleT<ISlot> slotTo;

    bool result = false;
    while (true)
    {
        auto nodeIterFrom = std::find_if(m_nodesModel.begin(), m_nodesModel.end(), [nodeIdFrom](const ObjectHandleT<INode> &node) {
            return nodeIdFrom == node->Id();
        });
        if (nodeIterFrom == m_nodesModel.end())
        {
            break;
        }

        nodeFrom = *nodeIterFrom;
        slotFrom = nodeFrom->GetSlotById(slotIdFrom);
        if (slotFrom == nullptr)
        {
            break;
        }            

        auto nodeIterTo = std::find_if(m_nodesModel.begin(), m_nodesModel.end(), [nodeIdTo](const ObjectHandleT<INode> &node) {
            return nodeIdTo == node->Id();
        });
        if (nodeIterTo == m_nodesModel.end())
        {
            break;
        }            

        nodeTo = *nodeIterTo;
        slotTo = nodeTo->GetSlotById(slotIdTo);
        if (slotTo == nullptr)
        {
            break;
        }            

        result = true;
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

    if (connectionPos == m_connectionsModel.end())
    {
        NGT_ERROR_MSG("Failed to delete connection with ID: %d", connectionId);
        return;
    }
    m_connectionsModel.erase(connectionPos);
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