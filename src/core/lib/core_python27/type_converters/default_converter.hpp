#pragma once

#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace PyScript
{
class ScriptObject;
} // namespace PyScript
namespace ReflectedPython
{
class DefinedInstance;
} // namespace ReflectedPython
class Variant;


namespace PythonType
{


/**
 *	Attempts to convert ScriptObject<->Variant.
 *	This is for any Python type that inherits from "object".
 */
class DefaultConverter
{
public:
	DefaultConverter( IComponentContext & context );

	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant,
		const ReflectedPython::DefinedInstance & parent,
		const std::string & childPath ) const;

	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) const;

	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) const;

private:
	IComponentContext & context_;
};


} // namespace PythonType

