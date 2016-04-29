
#include "CustomNode.h"

CustomNode::CustomNode(const std::string &nodeClass)
: m_x(0.0f)
, m_y(0.0f)
, m_class(nodeClass)
, m_title(nodeClass)  //TODO: change to title (Was done for demo)
, m_subTitle(nodeClass)  //TODO: change to subtitle (Was done for demo)
{
    m_id = std::to_string(reinterpret_cast<size_t>(this));

    int i = 0;
    while (i < 2)
    {
        m_inputSlotsModel.emplace_back(new CustomSlot(this, true));
        ++i;
    }

    i = 0;
    while (i < 1)
    {
        m_outputSlotsModel.emplace_back(new CustomSlot(this, false));
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

ObjectHandleT<ISlot> CustomNode::GetSlotByLabel(std::string) const
{
    //TODO: Need implementation
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
    //TODO: Need implementation
    return true; 
}

void CustomNode::OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    //TODO: Need implementation
}

void CustomNode::OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    //TODO: Need implementation
}