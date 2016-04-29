
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
    virtual void onDeleteNode(std::string nodeID) = 0;

    virtual void onCreateConnection(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot) = 0;
    virtual void onDeleteConnection(ObjectHandleT<IConnection> connection) = 0;

    virtual const IListModel* GetGraphModel() const = 0;

private:
    void onCreateConnectionInvoked()
    {
        assert(m_outputSlotNeedConnect != nullptr && m_inputSlotNeedConnect != nullptr);
        onCreateConnection(m_outputSlotNeedConnect, m_inputSlotNeedConnect);

        m_outputSlotNeedConnect = nullptr;
        m_inputSlotNeedConnect = nullptr;
    }

    void onDeleteConnectionInvoked()
    {
        assert(m_connectionNeedDelete != nullptr);
        onDeleteConnection(m_connectionNeedDelete);
        m_connectionNeedDelete = nullptr;
    }

private:
    ObjectHandleT<ISlot> m_outputSlotNeedConnect;
    ObjectHandleT<ISlot> m_inputSlotNeedConnect;
    ObjectHandleT<IConnection> m_connectionNeedDelete;
};

#endif // __I_NODE_EDITOR_H__