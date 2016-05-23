
#ifndef __I_NODE_H__
#define __I_NODE_H__

#include <string>
#include <qcolor.h>

#include "core_reflection/reflected_object.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/object_handle.hpp"

class IGraph;
class ISlot;

class INode
{
    DECLARE_REFLECTED
public:
	INode(){}
    virtual ~INode(){}

    virtual size_t Id() const = 0;
    virtual std::string Class() const = 0;
    virtual std::string Title() const = 0;
    virtual std::string SubTitle() const = 0;
    virtual std::string Category() const = 0;
    virtual std::string Icon() const = 0;

    virtual QColor Color() const = 0;

    virtual float X() const = 0;
    virtual float Y() const = 0;
    virtual void SetPos(float x, float y) = 0;
    
    virtual ObjectHandleT<ISlot> GetSlotById(size_t slotId) const = 0;

    virtual bool Enabled() const = 0;
    virtual void SetEnabled(bool) = 0;

    virtual bool Minimized() const = 0;
    virtual void SetMinimized(bool) = 0;

    virtual bool CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) = 0;
    virtual bool Validate(std::string &errorMessage) = 0;

    virtual void OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) = 0;
    virtual void OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) = 0;

    virtual const GenericListT<ObjectHandleT<ISlot>>* GetInputSlots() const = 0;
    virtual const GenericListT<ObjectHandleT<ISlot>>* GetOutputSlots() const = 0;

protected:
    //Models for QML
    virtual const IListModel* GetInputSlotsModel() const = 0;
    virtual const IListModel* GetOutputSlotsModel() const = 0;
};

#endif // __I_NODE_H__