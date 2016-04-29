
#include "CustomConnection.h"

ISlot* CustomConnection::Input() const
{
    return m_inputSlot.get();
}

ISlot* CustomConnection::Output() const
{
    return m_outputSlot.get();
}

bool CustomConnection::Bind(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot)
{
    bool result = false;
    if (outputSlot == nullptr || inputSlot == nullptr || m_isConnected)
    {
        return result;
    }

    while (true)
    {
        result = outputSlot->CanConnect(inputSlot);

        if (!result)
            break;

        result = inputSlot->CanConnect(outputSlot);
        
        if (!result)
            break;

        if (outputSlot->Connect(inputSlot) && inputSlot->Connect(outputSlot))
        {
            m_isConnected = true;
            m_inputSlot = inputSlot;
            m_outputSlot = outputSlot;
        }            
        else
        {
            outputSlot->Disconnect(inputSlot);
            inputSlot->Disconnect(outputSlot);
            result = false;
        }

        break;
    }

    return result;
}