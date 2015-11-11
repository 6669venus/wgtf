#pragma once
#ifndef PYTHON_SCRIPTING_ENGINE_HPP
#define PYTHON_SCRIPTING_ENGINE_HPP

#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "core_script/type_converter_queue.hpp"
#include "type_converters/long_converter.hpp"
#include "type_converters/string_converter.hpp"
#include "type_converters/type_converter.hpp"

#include <memory>


class IComponentContext;
class IDefinitionManager;
class IObjectManager;
class MetaType;
class Variant;
class ObjectHandle;

namespace PyScript
{
	class ScriptObject;
} // namespace PyScript


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


/**
 *	Interface to Python 2.7.x.
 */
class Python27ScriptingEngine
	: public Implements< IPythonScriptingEngine > // Always implement latest version
{
public:
	Python27ScriptingEngine( IComponentContext & context );
	virtual ~Python27ScriptingEngine();


	/**
	 *	The Python interpreter must be initialized before it can be used.
	 *	@return true on success.
	 */
	bool init();


	/**
	 *	Free all resources used by Python before execution ends.
	 *	If the interpreter was not initialized, it should do nothing.
	 */
	void fini();

	bool appendPath( const wchar_t* path ) override;
	ObjectHandle import( const char* name ) override;
	bool checkErrors() override;

private:
	Python27ScriptingEngine( const Python27ScriptingEngine & other );
	Python27ScriptingEngine( Python27ScriptingEngine && other );

	Python27ScriptingEngine & operator=( const Python27ScriptingEngine & other );
	Python27ScriptingEngine & operator=( Python27ScriptingEngine && other );


	std::vector< std::unique_ptr< MetaType > > defaultMetaTypes_;

	PythonType::StringConverter defaultTypeConverter_;
	PythonType::TypeConverter typeTypeConverter_;
	PythonType::LongConverter longTypeConverter_;
	PythonTypeConverters typeConverters_;
	IInterface * pTypeConvertersInterface_;
	IComponentContext & context_;
};


#endif // PYTHON_SCRIPTING_ENGINE_HPP

