#include "panel_context.hpp"


PanelContext::PanelContext()
	: pMapsSettingsXMLDataModel_( nullptr )
{
}


IListModel * PanelContext::getMapsSettingsXMLDataModel() const
{
	return pMapsSettingsXMLDataModel_;
}


const std::string & PanelContext::getTitle() const
{
	return title_;
}
