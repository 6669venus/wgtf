
#include "CustomSlot.h"

CustomSlot::CustomSlot(INode *node, bool isInput)
: m_isInput(isInput)
, m_pNode(node)
{
    m_id = reinterpret_cast<size_t>(this);
}

std::string CustomSlot::Label() const
{
    //TODO: Need implementation
    return std::string();
}

std::string CustomSlot::Icon() const
{
    //TODO: Need implementation
    return std::string();
}

QColor CustomSlot::Color() const
{
    //TODO: Need implementation
    return QColor();
}

MetaType* CustomSlot::Type() const
{
    //TODO: Need implementation
    return nullptr;
}

bool CustomSlot::Editable() const
{
    //TODO: Need implementation
    return false;
}

INode* CustomSlot::Node() const
{
    return m_pNode;
}

const IListModel* CustomSlot::GetConnectedSlots() const
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
    auto slotPos = std::find_if(m_connectedSlots.begin(), m_connectedSlots.end(), [slot](ObjectHandleT<ISlot> connectedSlot) {
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