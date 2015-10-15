#pragma once
#ifndef PYTHON_SCRIPTING_ENGINE_HPP
#define PYTHON_SCRIPTING_ENGINE_HPP

#include "interfaces/core_python_script/i_scripting_engine.hpp"

#include <memory>


class IDefinitionManager;
class IObjectManager;
class Variant;

namespace PyScript
{
	class ScriptObject;
} // namespace PyScript


/**
 *	Interface to Python 2.7.x.
 */
class Python27ScriptingEngine
	: public Implements< IPythonScriptingEngine > // Always implement latest version
{
public:
	Python27ScriptingEngine();
	virtual ~Python27ScriptingEngine();

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

	void registerTypeConverter( IPythonTypeConverter & converter ) override;
	void deregisterTypeConverter( IPythonTypeConverter & converter ) override;

	/**
	 *	Convert the given Variant into a ScriptObject by searching through the
	 *	type converters.
	 *	
	 *	Search is performed from most-recently-added to first-added.
	 *	
	 *	@param inVariant the variant to be converted.
	 *	@param outObject storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	bool toScriptObject( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) const;


	/**
	 *	Convert the given ScriptObject into a Variant by searching through the
	 *	type converters.
	 *	
	 *	Search is performed from most-recently-added to first-added.
	 *	
	 *	@param inObject the ScriptObject to be converted.
	 *	@param outVariant storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) const;

private:
	Python27ScriptingEngine( const Python27ScriptingEngine& other );
	Python27ScriptingEngine( Python27ScriptingEngine&& other );

	Python27ScriptingEngine& operator=( const Python27ScriptingEngine& other );
	Python27ScriptingEngine& operator=( Python27ScriptingEngine&& other );

	class Implementation;
	std::unique_ptr< Implementation > impl_;
};


#endif // PYTHON_SCRIPTING_ENGINE_HPP

