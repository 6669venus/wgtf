#pragma once
#ifndef PYTHON_PRIMITIVE_CONVERTER_HPP
#define PYTHON_PRIMITIVE_CONVERTER_HPP

#include "i_type_converter.hpp"

namespace PythonType
{


/**
 *	Attempts to convert ScriptObject<->T<->Variant.
 */
template< typename T >
class PrimitiveConverter : public IConverter
{
public:
	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
};


} // namespace PythonType

#endif // PYTHON_PRIMITIVE_CONVERTER_HPP

