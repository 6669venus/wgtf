
#ifndef __CUSTOM_GRAPH_H__
#define __CUSTOM_GRAPH_H__

#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/generic_list.hpp"

#include "plugins/plg_node_editor/interfaces/i_graph.hpp"
#include "plugins/plg_node_editor/interfaces/i_node.hpp"
#include "plugins/plg_node_editor/interfaces/i_connection.hpp"

class CustomGraph : public Implements<IGraph>
{
    DECLARE_REFLECTED
public:
    virtual ~CustomGraph(){}

    std::shared_ptr<INode> CreateNode(std::string nodeClass, float x = 0.0f, float y = 0.0f) override;
    void DeleteNode(size_t nodeID) override;
   
    ObjectHandleT<IConnection> CreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo) override;
    void DeleteConnection(size_t connectionId) override;
    
    bool Validate(std::string& errorMessage) override;
    void Save(std::string fileName) override;
    void Load(std::string fileName) override;

private:
    GenericListT<ObjectHandleT<INode>>::Iterator GetNodeIterById(size_t nodeId);
    const IListModel* GetNodesModel() const override { return &m_nodesModel; }
    const IListModel* GetConnectionsModel() const override { return &m_connectionsModel; }
private:
    GenericListT<ObjectHandleT<INode>> m_nodesModel;
    GenericListT<ObjectHandleT<IConnection>> m_connectionsModel;
};

#endif //__CUSTOM_GRAPH_H__