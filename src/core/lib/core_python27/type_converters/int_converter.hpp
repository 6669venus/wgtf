#pragma once
#ifndef PYTHON_INT_CONVERTER_HPP
#define PYTHON_INT_CONVERTER_HPP

#include "i_type_converter.hpp"

namespace PythonType
{


/**
 *	Attempts to convert ScriptObject<->int<->Variant.
 */
class IntConverter : public IConverter
{
public:
	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
};


} // namespace PythonType

#endif // PYTHON_INT_CONVERTER_HPP

