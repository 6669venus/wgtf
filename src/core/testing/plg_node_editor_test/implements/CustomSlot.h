
#ifndef __CUSTOM_SLOT_H__
#define __CUSTOM_SLOT_H__

#include "plugins/plg_node_editor/interfaces/i_slot.hpp"
#include "plugins/plg_node_editor/interfaces/i_connection.hpp"


class CustomSlot : public Implements<ISlot>
{
    DECLARE_REFLECTED
public:
    CustomSlot(INode *node, bool isInput);
    virtual ~CustomSlot(){}

    size_t Id() const override { return m_id; }
    bool IsInput() const override { return m_isInput; }
    std::string Label() const override;
    std::string Icon() const override;
    QColor Color() const override;
    MetaType* Type() const override;
    bool Editable() const override;
    INode* Node() const override;

    const std::set<size_t>* GetConnectionIds() const  override { return &m_connectionIds; }
    const GenericListT<ISlot*>* GetConnectedSlots() const override;

    bool CanConnect(ObjectHandleT<ISlot> slot) override;
    bool isConnected() const override { return !m_connectedSlots.empty(); }

    bool Connect(size_t connectionID, ObjectHandleT<ISlot> slot) override;
    bool Disconnect(size_t connectionID, ObjectHandleT<ISlot> slot) override;
private:
    size_t m_id;
    bool m_isInput;

    INode *m_pNode;
    std::set<size_t> m_connectionIds;
    GenericListT<ISlot*> m_connectedSlots;    
};

#endif //__CUSTOM_SLOT_H__