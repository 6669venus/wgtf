
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
        NGT_ERROR_MSG("Connection is corrupted");
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
        NGT_WARNING_MSG("Input arguments are null");
        return result;
    }

    if (isConnected)
    {
        NGT_WARNING_MSG("Connection is already connected");
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
            outputSlot->Disconnect(inputSlot);
            inputSlot->Disconnect(outputSlot);
            result = false;
        }

        break;
    }

    return result;
}