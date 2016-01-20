#pragma once
#ifndef _PANEL_CONTEXT_HPP
#define _PANEL_CONTEXT_HPP

#include "core_reflection/reflected_object.hpp"

#include <string>


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
	const std::string & getTitle() const;
	
	IListModel * pMapsSettingsXMLDataModel_;
	std::string title_;
};


#endif // _PANEL_CONTEXT_HPP
