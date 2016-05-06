
#include "node_editor.hpp"

void NodeEditor::SetGraph(std::shared_ptr<IGraph> graph)
{
    if (graph == nullptr)
    {
        return;
    }
    
    if (graphModel.canClear())
    {
        graphModel.clear();
    }

    graphModel.push_back(graph);
}

std::shared_ptr<INode> NodeEditor::CreateNode(std::string nodeClass, float x, float y)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return nullptr;
}

void NodeEditor::onCreateNode(int x, int y, std::string nodeClass)
{
    std::shared_ptr<INode> node = graphModel.back()->CreateNode(nodeClass, x, y);
}

void NodeEditor::onDeleteNode(size_t nodeID)
{
    graphModel.back()->DeleteNode(nodeID);
}

void NodeEditor::onCreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo)
{
    graphModel.back()->CreateConnection(nodeIdFrom, slotIdFrom, nodeIdTo, slotIdTo);
}

void NodeEditor::onDeleteConnection(size_t connectionId)
{
    graphModel.back()->DeleteConnection(connectionId);
}

INode* NodeEditor::GetNode(std::string id)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return nullptr;
}

bool NodeEditor::DeleteNode(std::string id)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return true;
}

bool NodeEditor::Connect(std::string nodeIdFrom, std::string slotLabelFrom,
	std::string nodeIdTo, std::string slotLabelTo)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return true;
}

bool NodeEditor::Disconnect(std::string nodeIdFrom, std::string slotLabelFrom,
	std::string nodeIdTo, std::string slotLabelTo)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return true;
}