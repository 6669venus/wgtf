#pragma once
#ifndef _BALANCE_PANEL_CONTEXT_HPP
#define _BALANCE_PANEL_CONTEXT_HPP

#include "core_reflection/reflected_object.hpp"

class IListModel;


/**
 *	Stores variables that can be accessed by QML.
 */ 
class BalancePanelContext
{
public:
	DECLARE_REFLECTED
	BalancePanelContext();

	IListModel * getMapsSettingsXMLDataModel() const;
	
	IListModel * pMapsSettingsXMLDataModel_;
};


#endif // _BALANCE_PANEL_CONTEXT_HPP
