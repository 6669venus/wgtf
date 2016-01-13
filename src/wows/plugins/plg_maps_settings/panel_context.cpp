#include "panel_context.hpp"


PanelContext::PanelContext()
	: pMapsSettingsXMLDataModel_( nullptr )
{
}


IListModel * PanelContext::getMapsSettingsXMLDataModel() const
{
	return pMapsSettingsXMLDataModel_;
}

