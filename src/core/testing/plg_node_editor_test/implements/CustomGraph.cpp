
#include "CustomGraph.h"
#include "CustomNode.h"
#include "CustomConnection.h"

std::shared_ptr<INode> CustomGraph::CreateNode(std::string nodeClass, float x, float y)
{  
    std::shared_ptr<INode> node(new CustomNode(nodeClass));
    node->SetPos(x, y);
    m_nodesModel.emplace_back(node);
    
    return node;
}

void CustomGraph::DeleteNode(const std::string &nodeID)
{
    auto nodePos = std::find_if(m_nodesModel.begin(), m_nodesModel.end(), [&nodeID](ObjectHandleT<INode> node) {
            return nodeID == node->Id();
    });

    if (nodePos != m_nodesModel.end())
    {
        m_nodesModel.erase(nodePos);
    }        
}

ObjectHandleT<IConnection> CustomGraph::CreateConnection(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot)
{
    ObjectHandleT<IConnection> connection(new CustomConnection());    
    bool result = connection->Bind(outputSlot, inputSlot);

    if (result)
    {
        m_connectionsModel.emplace_back(connection.get());
        return connection;
    }

    return nullptr;
}

void CustomGraph::DeleteConnection(ObjectHandleT<IConnection> deleteConnection)
{
    auto connectionPos = std::find_if(m_connectionsModel.begin(), m_connectionsModel.end(), [deleteConnection](ObjectHandleT<IConnection> connection) {
        return deleteConnection.get() == connection.get();
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