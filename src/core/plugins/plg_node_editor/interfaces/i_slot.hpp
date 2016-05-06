
#ifndef __I_SLOT_H__
#define __I_SLOT_H__

#include <string>
#include <qcolor.h>

#include "core_reflection/reflected_object.hpp"
#include "core_data_model/i_list_model.hpp"

#include "i_node.hpp"
#include <set>

class MetaType;

class ISlot
{
    DECLARE_REFLECTED
public:
    ISlot() = default;
    virtual ~ISlot(){}

    ISlot(const ISlot&) = delete;
    ISlot& operator=(const ISlot&) = delete;

    virtual size_t Id() const = 0;
    virtual bool IsInput() const = 0;
    virtual std::string Label() const = 0;
    virtual std::string Icon() const = 0;
    virtual QColor Color() const = 0;
    virtual MetaType* Type() const = 0;
    virtual bool Editable() const = 0;
    virtual INode* Node() const = 0;

    virtual bool isConnected() const = 0;
    virtual const std::set<size_t>* GetConnectionIds() const = 0;

    virtual const GenericListT<ISlot*>* GetConnectedSlots() const = 0;
    virtual bool CanConnect(ObjectHandleT<ISlot> slot) = 0;

    virtual bool Connect(size_t connectionId, ObjectHandleT<ISlot> slot) = 0;
    virtual bool Disconnect(size_t connectionId, ObjectHandleT<ISlot> slot) = 0;
};

#endif //__I_SLOT_H__