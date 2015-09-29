#pragma once
#ifndef PYTHON_SCRIPTING_ENGINE_HPP
#define PYTHON_SCRIPTING_ENGINE_HPP

#include "interfaces/core_python_script/i_scripting_engine.hpp"

class IDefinitionManager;
class IObjectManager;

/**
 *	Interface to Python 2.7.x.
 */
class Python27ScriptingEngine
	: public Implements< IPythonScriptingEngine > // Always implement latest version
{
public:
	/**
	 *	The Python interpreter must be initialized before it can be used.
	 *	@return true on success.
	 */
	bool init( IDefinitionManager& definitionManager,
		IObjectManager& objectManager );


	/**
	 *	Free all resources used by Python before execution ends.
	 *	If the interpreter was not initialized, it should do nothing.
	 */
	void fini();

	bool appendPath( const wchar_t* path ) override;
	std::shared_ptr< IPythonModule > import( const char* name ) override;
};


#endif // PYTHON_SCRIPTING_ENGINE_HPP

