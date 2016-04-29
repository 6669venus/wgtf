
#ifndef __I_GRAPH_H__
#define __I_GRAPH_H__

#include <string>
#include "core_reflection/reflected_object.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/object_handle.hpp"

#include "i_node.hpp"
#include "i_connection.hpp"

class IGraph
{
    DECLARE_REFLECTED
public:
    virtual ~IGraph(){}

    virtual std::shared_ptr<INode> CreateNode(std::string, float x = 0.0f, float y = 0.0f) = 0;
    virtual void DeleteNode(size_t nodeID) = 0;

    virtual ObjectHandleT<IConnection> CreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo) = 0;
    virtual void DeleteConnection(size_t connectionId) = 0;
    
    virtual bool Validate(std::string &errorMessage) = 0;

    virtual void Save(std::string fileName) = 0;
    virtual void Load(std::string fileName) = 0;

protected:
    virtual const IListModel* GetNodesModel() const = 0;
    virtual const IListModel* GetConnectionsModel() const = 0;
};

#endif // __I_GRAPH_H__