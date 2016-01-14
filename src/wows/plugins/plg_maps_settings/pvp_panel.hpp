#pragma once
#ifndef _PVP_PANEL_HPP
#define _PVP_PANEL_HPP

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

class PvpPanel : public Depends< DEPENDS_ON_CLASSES >
{
	typedef Depends< DEPENDS_ON_CLASSES > DepsBase;

public:
	PvpPanel( IComponentContext & context );
	virtual ~PvpPanel();

private:
	/**
	 *	Create the context object to provide access to the Python references from QML.
	 *	Also registers definitions of reflected objects with the reflection system.
	 *	@return true on success.
	 */
	bool createDataModel( IComponentContext & context );
	void destroyDataModel();

	/**
	 *	Import a module from Python and take references to Python objects.
	 *	@return true on success.
	 */
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
	ObjectHandle rootPythonObject_;
	Collection mapsSettingsXMLDataCollection_;

	// Holds data model, provides access to QML
	ObjectHandleT< PanelContext > contextObject_;

	std::unique_ptr< IView > pvpPanelView_;
};

#endif // _PVP_PANEL_HPP
