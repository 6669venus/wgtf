
#include "CustomNode.h"

CustomNode::CustomNode(const std::string &nodeClass)
: m_x(0.0f)
, m_y(0.0f)
, m_class(nodeClass)
, m_title(nodeClass)  //TODO: change to title (Was done for demo)
, m_subTitle(nodeClass)  //TODO: change to subtitle (Was done for demo)
{
    m_id = reinterpret_cast<size_t>(this);

    int i = 0;
    while (i < 2)
    {
        m_inputSlotsModel.push_back(new CustomSlot(this, true));
        ++i;
    }

    i = 0;
    while (i < 1)
    {
        m_outputSlotsModel.push_back(new CustomSlot(this, false));
        ++i;
    }
}

void CustomNode::SetPos(float x, float y)
{
    m_x = x;
    m_y = y;
}

ObjectHandleT<ISlot> CustomNode::GetSlotById(size_t slotId) const
{
    auto slotPos = std::find_if(m_inputSlotsModel.begin(), m_inputSlotsModel.end(), [slotId](const ObjectHandleT<ISlot> &slot) {
        return slotId == slot->Id();
    });

    if (slotPos != m_inputSlotsModel.end())
    {
        return *slotPos;
    }

    slotPos = std::find_if(m_outputSlotsModel.begin(), m_outputSlotsModel.end(), [slotId](const ObjectHandleT<ISlot> &slot) {
        return slotId == slot->Id();
    });

    if (slotPos != m_outputSlotsModel.end())
    {
        return *slotPos;
    }

    return nullptr;
}

bool CustomNode::Enabled() const
{
    //TODO: Need implementation
    return true;
}

void CustomNode::SetEnabled(bool)
{
    //TODO: Need implementation
}

bool CustomNode::Minimized() const
{
    //TODO: Need implementation
    return true;
}

void CustomNode::SetMinimized(bool)
{
    //TODO: Need implementation
}

bool CustomNode::Validate(std::string &errorMessage)
{
    //TODO: Need implementation
    return true;
}

bool CustomNode::CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) 
{ 
    bool result = false;

    while (true)
    {
        ObjectHandleT<INode> otherNode = otherSlot->Node();
        if (this == otherNode.get())
            break;

        if (!(mySlot->IsInput() ^ otherSlot->IsInput()))
            break;

        result = true;
        break;
    }

    return result;
}

void CustomNode::OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    //TODO: Need implementation
}

void CustomNode::OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    //TODO: Need implementation
}