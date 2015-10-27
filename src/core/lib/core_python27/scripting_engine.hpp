#pragma once
#ifndef PYTHON_SCRIPTING_ENGINE_HPP
#define PYTHON_SCRIPTING_ENGINE_HPP

#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "core_script/type_converter_queue.hpp"
#include "type_converters/long_converter.hpp"
#include "type_converters/string_converter.hpp"

#include <memory>


class IDefinitionManager;
class IObjectManager;
class Variant;

namespace PyScript
{
	class ScriptObject;
} // namespace PyScript

class ReflectionModule;


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


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
	bool init( IComponentContext & context );


	/**
	 *	Free all resources used by Python before execution ends.
	 *	If the interpreter was not initialized, it should do nothing.
	 */
	void fini( IComponentContext & context );

	bool appendPath( const wchar_t* path ) override;
	std::shared_ptr< IPythonModule > import( const char* name ) override;

private:
	Python27ScriptingEngine( const Python27ScriptingEngine & other );
	Python27ScriptingEngine( Python27ScriptingEngine && other );

	Python27ScriptingEngine & operator=( const Python27ScriptingEngine & other );
	Python27ScriptingEngine & operator=( Python27ScriptingEngine && other );

	std::unique_ptr< ReflectionModule > reflectionModule_;

	PythonType::StringConverter defaultTypeConverter_;
	PythonType::LongConverter longTypeConverter_;
	PythonTypeConverters typeConverters_;
	IInterface * pTypeConvertersInterface_;
};


#endif // PYTHON_SCRIPTING_ENGINE_HPP

