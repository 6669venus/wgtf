#pragma once
#ifndef PYTHON_TYPE_CONVERTER_HPP
#define PYTHON_TYPE_CONVERTER_HPP

#include "i_type_converter.hpp"


/**
 *	Attempts to convert ScriptObject<->string<->Variant.
 */
class StringTypeConverter : public IPythonTypeConverter
{
public:
	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
};


#endif // PYTHON_TYPE_CONVERTER_HPP

