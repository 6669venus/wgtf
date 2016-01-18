#pragma once
#ifndef _PYTHON_LOADER_HPP
#define _PYTHON_LOADER_HPP

#include "core_dependency_system/depends.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/object_handle.hpp"


class IDefinitionManager;
class IReflectionController;


#define DEPENDS_ON_CLASSES \
	IDefinitionManager, \
	IReflectionController, \
	IPythonScriptingEngine, 

class PythonLoader : public Depends< DEPENDS_ON_CLASSES >
{
	typedef Depends< DEPENDS_ON_CLASSES > DepsBase;

public:
	PythonLoader( IComponentContext & context );
	virtual ~PythonLoader();

	/**
	 *	Import a module from Python and take references to Python objects.
	 *	@return true on success.
	 */
	ObjectHandle createPythonObjects();

	/**
	 *	Drop references to Python objects.
	 */
	void destroyPythonObjects();

private:

	// Holds references to Python objects
	ObjectHandle rootPythonObject_;
};

#endif // _PYTHON_LOADER_HPP

