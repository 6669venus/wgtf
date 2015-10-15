#pragma once
#ifndef PYTHON_TYPE_CONVERTER_HPP
#define PYTHON_TYPE_CONVERTER_HPP

#include "interfaces/core_python_script/i_type_converter.hpp"


/**
 *	Attempts to convert ScriptObject<->string<->Variant.
 */
class StringTypeConverter
	: public Implements< IPythonTypeConverter > // Always implement latest version
{
public:
	bool toVariant( const PyScript::ScriptObject& inObject,
		Variant& outVariant ) override;
	bool toScriptObject( const Variant& inVariant,
		PyScript::ScriptObject& outObject ) override;
};


#endif // PYTHON_TYPE_CONVERTER_HPP

