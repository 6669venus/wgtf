
#ifndef __DEFAULT_NODE_EDITOR_H__
#define __DEFAULT_NODE_EDITOR_H__

#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_data_model/generic_list.hpp"

#include "interfaces/i_node_editor.hpp"

class NodeEditor : public Implements<INodeEditor>
{
	DECLARE_REFLECTED
public:
    virtual ~NodeEditor(){}
	
    void SetGraph(std::shared_ptr<IGraph> graph) override;

	std::shared_ptr<INode> CreateNode(std::string nodeClass, float x, float y) override;
	INode* GetNode(std::string id) override;
	bool DeleteNode(std::string id) override;    

	bool Connect(std::string nodeIdFrom, std::string slotLabelFrom,
		std::string nodeIdTo, std::string slotLabelTo) override;
	bool Disconnect(std::string nodeIdFrom, std::string slotLabelFrom,
		std::string nodeIdTo, std::string slotLabelTo) override;

private:
    void onCreateNode(int x, int y, std::string nodeClass) override;
    void onDeleteNode(std::string nodeID) override;

    void onCreateConnection(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot) override;
    void onDeleteConnection(ObjectHandleT<IConnection> connection) override;

    const IListModel* GetGraphModel() const override { return &graphModel; }

private:
    GenericListT<ObjectHandleT<IGraph>> graphModel;
};

#endif // __DEFAULT_NODE_EDITOR_H__