
#include "CustomConnection.h"
#include "core_logging\logging.hpp"

CustomConnection::CustomConnection() 
: m_inputSlot(nullptr)
, m_outputSlot(nullptr)
, isConnected(false)
{
    m_id = reinterpret_cast<size_t>(this);
}

CustomConnection::~CustomConnection()    
{
    if (m_inputSlot != nullptr && m_outputSlot != nullptr)
    {
        m_inputSlot->Disconnect(m_outputSlot);
        m_outputSlot->Disconnect(m_inputSlot);
    }
    else if (m_inputSlot != nullptr || m_outputSlot != nullptr)
    {
        NGT_ERROR_MSG("Connection is corrupted\n");
    }
}

ISlot* CustomConnection::Input() const
{
    return m_inputSlot;
}

ISlot* CustomConnection::Output() const
{
    return m_outputSlot;
}

bool CustomConnection::Bind(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot)
{
    bool result = false;
    if (outputSlot == nullptr || inputSlot == nullptr)
    {
        NGT_ERROR_MSG("Input arguments are null\n");
        return result;
    }

    if (isConnected)
    {
        NGT_ERROR_MSG("Connection is already connected\n");
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
            m_inputSlot = inputSlot.get();
            m_outputSlot = outputSlot.get();
            isConnected = true;
        }            
        else
        {
            result = false;
            NGT_ERROR_MSG("Failed to connect input and output slots\n");

            outputSlot->Disconnect(inputSlot);
            inputSlot->Disconnect(outputSlot);                        
        }

        break;
    }

    return result;
}