#pragma once

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_python_script/i_scripting_engine.hpp"


#include <memory>


class IDefinitionManager;
class IReflectionController;
class IUIFramework;
class IUIApplication;
struct PythonObjects;


#define DEPENDS_ON_CLASSES \
	IDefinitionManager, \
	IReflectionController, \
	IPythonScriptingEngine, \
	IUIFramework, \
	IUIApplication


class PythonPanel: Depends<DEPENDS_ON_CLASSES>
{
	typedef Depends<DEPENDS_ON_CLASSES> DepsBase;

public:
	PythonPanel( IComponentContext& context );

	void initialise();
	void finalise();

private:
	bool createContextObject();
	bool createPythonObjects();
	bool addPanel();
	void removePanel();
	void destroyPythonObjects();

	std::unique_ptr<IView> pythonView_;
	ObjectHandleT<PythonObjects> contextObject_;
};
