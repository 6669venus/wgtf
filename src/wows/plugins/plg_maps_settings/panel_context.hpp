#pragma once
#ifndef _BALANCE_PANEL_CONTEXT_HPP
#define _BALANCE_PANEL_CONTEXT_HPP

#include "core_reflection/reflected_object.hpp"

class IListModel;


/**
 *	Stores variables that can be accessed by QML.
 */ 
class PanelContext
{
public:
	DECLARE_REFLECTED
	PanelContext();

	IListModel * getMapsSettingsXMLDataModel() const;
	
	IListModel * pMapsSettingsXMLDataModel_;
};


#endif // _BALANCE_PANEL_CONTEXT_HPP
