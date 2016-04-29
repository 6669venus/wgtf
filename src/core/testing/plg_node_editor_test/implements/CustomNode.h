
#ifndef __CUSTOM_NODE_H__
#define __CUSTOM_NODE_H__

#include "core_data_model/i_list_model.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/object_handle.hpp"

#include "plugins/plg_node_editor/interfaces/i_node.hpp"

#include "CustomSlot.h"
#include "CustomGraph.h"

class CustomNode : public Implements<INode>
{
    DECLARE_REFLECTED
public:
    CustomNode(const std::string &nodeClass);
    virtual ~CustomNode();

    size_t Id() const override         { return m_id; }
    std::string Class() const override      { return m_class; }
    std::string Title() const override      { return m_title; }
    std::string SubTitle() const override   { return m_subTitle; }
    std::string Category() const override   { return m_category; }
    std::string Icon() const override       { return m_icon; }
    QColor Color() const override           { return m_color; }
    
    float X() const override                { return m_x; }
    float Y() const override                { return m_y; }
    void SetPos(float x, float y) override;

    ObjectHandleT<ISlot> GetSlotById(size_t slotId) const override;

    bool Enabled() const override;
    void SetEnabled(bool) override;

    bool Minimized() const override;
    void SetMinimized(bool) override;

    bool CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) override;
    bool Validate(std::string &errorMessage) override;

    void OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) override;
    void OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) override;

private:
    const IListModel* GetInputSlotsModel() const override { return &m_inputSlotsModel; }
    const IListModel* GetOutputSlotsModel() const override { return &m_outputSlotsModel; }

private:
    float m_x;
    float m_y;   

    size_t m_id;
    std::string m_class;
    std::string m_title;
    std::string m_subTitle;
    std::string m_category;
    std::string m_icon;

    QColor m_color;
    GenericListT<ObjectHandleT<ISlot>> m_inputSlotsModel;
    GenericListT<ObjectHandleT<ISlot>> m_outputSlotsModel;
};

#endif // __CUSTOM_NODE_H__