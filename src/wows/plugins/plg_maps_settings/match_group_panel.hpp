#pragma once
#ifndef _MATCH_GROUP_PANEL_HPP
#define _MATCH_GROUP_PANEL_HPP

#include "panel_context.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_variant/collection.hpp"

#include <memory>


class IDefinitionManager;
class IReflectionController;
class IUIFramework;
class IUIApplication;
class PanelContext;


#define DEPENDS_ON_CLASSES \
	IDefinitionManager, \
	IReflectionController, \
	IPythonScriptingEngine, \
	IUIFramework, \
	IUIApplication

class MatchGroupPanel : public Depends< DEPENDS_ON_CLASSES >
{
	typedef Depends< DEPENDS_ON_CLASSES > DepsBase;

public:
	MatchGroupPanel( IComponentContext & context,
		Collection & mapsSettingsXMLDataCollection,
		const char * title,
		const wchar_t * matchGroup );
	virtual ~MatchGroupPanel();

private:
	/**
	 *	Create the context object to provide access to the Python references from QML.
	 *	Also registers definitions of reflected objects with the reflection system.
	 *	@return true on success.
	 */
	bool createDataModel( IComponentContext & context,
		const char * title,
		const wchar_t * matchGroup );
	void destroyDataModel();

	bool createPythonObjects();
	/**
	 *	Drop references to Python objects.
	 */
	void destroyPythonObjects();

	/**
	 *	Create the panel and add it to the window. (The window exists in a different plugin)
	 *	@return true on success.
	 */
	bool addPanel();
	/**
	 *	Remove the panel from the window, delete the panel.
	 */
	void removePanel();

	// Holds references to Python objects
	Collection mapsSettingsXMLDataCollection_;

	// Holds data model, provides access to QML
	ObjectHandleT< PanelContext > contextObject_;

	std::unique_ptr< IView > pvpPanelView_;
};

#endif // _MATCH_GROUP_PANEL_HPP
