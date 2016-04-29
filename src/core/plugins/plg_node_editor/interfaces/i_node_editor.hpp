
#ifndef __I_NODE_EDITOR_H__
#define __I_NODE_EDITOR_H__

#include <string>
#include <memory>

#include "core_reflection/object_handle.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_reflection/reflected_object.hpp"

#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"

#include "i_graph.hpp"

class INodeEditor
{
	DECLARE_REFLECTED;
public:
	virtual ~INodeEditor(){}

    virtual void SetGraph(std::shared_ptr<IGraph>) = 0;

	virtual std::shared_ptr<INode> CreateNode(std::string nodeClass, float x, float y) = 0;
	virtual INode* GetNode(std::string id) = 0;
	virtual bool DeleteNode(std::string id) = 0;

	virtual bool Connect(std::string nodeIdFrom, std::string slotLabelFrom, 
		std::string nodeIdTo, std::string slotLabelTo) = 0;
	virtual bool Disconnect(std::string nodeIdFrom, std::string slotLabelFrom, 
		std::string nodeIdTo, std::string slotLabelTo) = 0;
    
protected:
    virtual void onCreateNode(int x, int y, std::string nodeClass) = 0;
    virtual void onDeleteNode(size_t nodeID) = 0;

    virtual void onCreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo) = 0;
    virtual void onDeleteConnection(size_t connectionId) = 0;

    virtual const IListModel* GetGraphModel() const = 0;
};

#endif // __I_NODE_EDITOR_H__