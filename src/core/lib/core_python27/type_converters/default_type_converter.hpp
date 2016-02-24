#pragma once

#include "i_type_converter.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"


namespace ReflectedPython
{
	class DefinedInstance;
} // namespace ReflectedPython


namespace PythonType
{
/**
 *	Attempts to convert ScriptObject<->Variant.
 */
class DefaultTypeConverter
{
public:
	DefaultTypeConverter( IComponentContext & context );

	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant,
		const ReflectedPython::DefinedInstance & parent,
		const std::string & childPath );

	bool toScriptType( const Variant & inVariant, PyScript::ScriptObject & outObject );

private:
	IComponentContext & context_;
};
} // namespace PythonType

