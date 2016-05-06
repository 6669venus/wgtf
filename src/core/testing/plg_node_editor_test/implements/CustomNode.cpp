
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

CustomNode::~CustomNode()
{}

void CustomNode::SetPos(float x, float y)
{
    m_x = x;
    m_y = y;
}

ObjectHandleT<ISlot> CustomNode::GetSlotById(size_t slotId) const
{
    auto inputSlotPos = std::find_if(m_inputSlotsModel.begin(), m_inputSlotsModel.end(), [slotId](const ObjectHandleT<ISlot> &inputSlot) {
        return slotId == inputSlot->Id();
    });

    if (inputSlotPos != m_inputSlotsModel.end())
    {
        return *inputSlotPos;
    }

    auto outputSlotPos = std::find_if(m_outputSlotsModel.begin(), m_outputSlotsModel.end(), [slotId](const ObjectHandleT<ISlot> &outputSlot) {
        return slotId == outputSlot->Id();
    });

    if (outputSlotPos != m_outputSlotsModel.end())
    {
        return *outputSlotPos;
    }

    NGT_ERROR_MSG("Slot with id: %d not found\n", slotId);
    return nullptr;
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

        auto connectedSlots = mySlot->GetConnectedSlots();
        result = (std::find_if(connectedSlots->begin(), connectedSlots->end(), [&otherSlot](const ISlot *slot){ return otherSlot->Id() == slot->Id(); }) == connectedSlots->end());
        break;
    }

    return result;
}

bool CustomNode::Enabled() const
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return true;
}

void CustomNode::SetEnabled(bool)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

bool CustomNode::Minimized() const
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return true;
}

void CustomNode::SetMinimized(bool)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

bool CustomNode::Validate(std::string &errorMessage)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return true;
}

void CustomNode::OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

void CustomNode::OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}