#pragma once
#ifndef _BALANCE_PANEL_HPP
#define _BALANCE_PANEL_HPP

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


/**
 *	The panel for editing the weights for the battle balancer.
 *	- Loads battle data from Python scripts
 *	- Constructs a data model from the Python scripts
 *	- Provides a UI for editing the data
 */
class BalancePanel : public Depends< DEPENDS_ON_CLASSES >
{
	typedef Depends< DEPENDS_ON_CLASSES > DepsBase;
public:
	BalancePanel( IComponentContext & context );
	~BalancePanel();

private:
	/**
	 *	Create the context object to provide access to the Python references from QML.
	 *	Also registers definitions of reflected objects with the reflection system.
	 *	@return true on success.
	 */
	bool createDataModel();
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

	// QML panel
	std::unique_ptr< IView > pythonView_;
};

#endif // _BALANCE_PANEL_HPP
