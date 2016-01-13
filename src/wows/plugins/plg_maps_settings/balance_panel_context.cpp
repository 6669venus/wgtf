#include "balance_panel_context.hpp"


BalancePanelContext::BalancePanelContext()
	: pMapsSettingsXMLDataModel_( nullptr )
{
}


IListModel * BalancePanelContext::getMapsSettingsXMLDataModel() const
{
	return pMapsSettingsXMLDataModel_;
}

