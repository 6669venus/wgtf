
#ifndef __CUSTOM_CONNECTION_H__
#define __CUSTOM_CONNECTION_H__

#include "plugins/plg_node_editor/interfaces/i_connection.hpp"

class CustomConnection : public Implements<IConnection>
{
    DECLARE_REFLECTED
public:
    CustomConnection() : m_isConnected(false), m_inputSlot(nullptr), m_outputSlot(nullptr)
    {
        m_id = reinterpret_cast<size_t>(this);
    }
    virtual ~CustomConnection(){}

    size_t Id() const override { return m_id; }
    ISlot* Input() const override;
    ISlot* Output() const override;
    bool Bind(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot) override;
private:
    size_t m_id;
    bool m_isConnected;
    ObjectHandleT<ISlot> m_inputSlot;
    ObjectHandleT<ISlot> m_outputSlot;
};

#endif //__CUSTOM_CONNECTION_H__