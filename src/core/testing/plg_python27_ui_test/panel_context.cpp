#include "panel_context.hpp"
#include "core_data_model/i_tree_model.hpp"

#include "metadata/panel_context.mpp"


PanelContext::PanelContext()
{
}


PanelContext::~PanelContext()
{
}


const std::string & PanelContext::panelName() const
{
	return panelName_;
}


ITreeModel * PanelContext::treeModel() const
{
	return treeModel_.get();
}

