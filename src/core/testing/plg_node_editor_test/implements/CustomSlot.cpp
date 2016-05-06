
#include "CustomSlot.h"

CustomSlot::CustomSlot(INode *node, bool isInput)
: m_isInput(isInput)
, m_pNode(node)
, m_pConnection(nullptr)
{
    m_id = reinterpret_cast<size_t>(this);
}

std::string CustomSlot::Label() const
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return std::string();
}

std::string CustomSlot::Icon() const
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return std::string();
}

QColor CustomSlot::Color() const
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return QColor();
}

MetaType* CustomSlot::Type() const
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return nullptr;
}

bool CustomSlot::Editable() const
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return false;
}

INode* CustomSlot::Node() const
{
    return m_pNode;
}

const GenericListT<ISlot*>* CustomSlot::GetConnectedSlots() const
{
    return &m_connectedSlots;
}

bool CustomSlot::CanConnect(ObjectHandleT<ISlot> slot)
{
    assert(m_pNode != nullptr);
    return m_pNode->CanConnect(this, slot);
}

bool CustomSlot::Connect(ObjectHandleT<ISlot> slot)
{
    assert(m_pNode != nullptr);
    bool result = m_pNode->CanConnect(this, slot);

    if (result)
    {
        m_connectedSlots.push_back(slot.get());
        m_pNode->OnConnect(this, m_connectedSlots.back());
    }

    return result;
}

bool CustomSlot::Disconnect(ObjectHandleT<ISlot> slot)
{
    assert(m_pNode != nullptr);
    bool result = false;
    auto slotPos = std::find_if(m_connectedSlots.begin(), m_connectedSlots.end(), [&slot](const ObjectHandleT<ISlot> &connectedSlot) {
        return slot->Id() == connectedSlot->Id();
    });

    if (slotPos != m_connectedSlots.end())
    {
        result = true;
        m_connectedSlots.erase(slotPos);
        m_pNode->OnDisconnect(this, slot);
    }

    return result;
}