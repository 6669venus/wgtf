
#ifndef __GRAPHEDITOR_CONNECTIONMANAGER_H__
#define __GRAPHEDITOR_CONNECTIONMANAGER_H__

#include <core_data_model/i_list_model.hpp>
#include <core_reflection/object_handle.hpp>
#include <core_reflection/i_definition_manager.hpp>

#include <vector>
#include <unordered_map>

class ConnectionSlot;
class Connector;
class GraphNode;

class ConnectionManager
{
    using TSlotPtr = ObjectHandleT<ConnectionSlot>;
    using TConnectorPtr = ObjectHandleT<Connector>;
    using TNodePtr = ObjectHandleT<GraphNode>;

    using TSlots = std::vector<TSlotPtr>;
    using TConnectors = std::vector<TConnectorPtr>;

public:
    static ConnectionManager& Instance();

    void Initialize(IDefinitionManager* defMng);
    void Finilize();

    ObjectHandleT<GraphNode> CreateNode(const std::string& nodeTypeId);
    void DeleteNode(ObjectHandleT<GraphNode> node);
    void DeleteNode(size_t nodeUid);

    ObjectHandleT<Connector> CreateConnection(size_t outputslotId, size_t inputSlotId);
    void DeleteConnection(Connector* connection);
    void Disconnect(size_t slotId);

    IListModel* GetNodeModel() const;
    IListModel* GetConnectorsModel() const;
    IListModel* GetRootContextMenuModel() const;
    IListModel* GetNodeContextMenuModel() const;
    IListModel* GetSlotContextMenuModel() const;

    ObjectHandleT<ConnectionSlot> GetSlot(size_t slotUID);

private:
    ConnectionManager();
    ~ConnectionManager();

    using TCreateSlotFn = std::function<ObjectHandleT<ConnectionSlot>(const ObjectHandleT<GraphNode>&,
                                                                      const std::string&,
                                                                      const std::string&)>;
    TNodePtr CreateNode(const std::string& nodeTypeId, const TCreateSlotFn& fn);

    TSlotPtr CreateSlot(TNodePtr const& parentNode, const std::string& title, const std::string& icon);
    IDefinitionManager& GetDefinitionManager();

    void InitRootContextMenu();
    void InitNodeContextMenu();
    void InitSlotContextMenu();

    void SaveGraph(float x, float y, size_t objectUid);
    void LoadGraph(float x, float y, size_t objectUid);
    void CreateNodeHelper(float x, float y, size_t objectUid, const std::string& typeId);

    void ClearModel();

    void GraphNodeChanged(GraphNode* node);

    void MoveHalf(float x, float y);

private:
    template <typename T>
    struct ObjectHandleHash
    {
        size_t operator()(ObjectHandleT<T> const& node) const
        {
            return std::hash<T*>()(node.get());
        }
    };

    using TNodeToSlotsMap = std::unordered_map<TNodePtr, TSlots, ObjectHandleHash<GraphNode>>;
    using TSlotToConnectorsMap = std::unordered_map<TSlotPtr, TConnectors, ObjectHandleHash<ConnectionSlot>>;
    using TUidToSlotMap = std::unordered_map<size_t, TSlotPtr>;

    TNodeToSlotsMap nodeToSlots;
    TSlotToConnectorsMap slotToConnector;
    TUidToSlotMap uidToSlot;

    IDefinitionManager* defMng = nullptr;

    class GraphNodeModel;
    class ConnectorsModel;
    class ContextMenuModel;

    std::unique_ptr<GraphNodeModel> graphNodeModel;
    std::unique_ptr<ConnectorsModel> connectorsModel;
    std::unique_ptr<ContextMenuModel> rootContextMenuModel;
    std::unique_ptr<ContextMenuModel> nodeContextMenuModel;
    std::unique_ptr<ContextMenuModel> slotContextMenuModel;
};

#endif // __GRAPHEDITOR_CONNECTIONMANAGER_H__