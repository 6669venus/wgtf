#pragma once
#ifndef PYTHON_SEQUENCE_CONVERTER_HPP
#define PYTHON_SEQUENCE_CONVERTER_HPP

#include "i_type_converter.hpp"

namespace PythonType
{


/**
 *	Attempts to convert ScriptObject<->sequence<->Variant.
 */
class SequenceConverter : public IConverter
{
public:
	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
};


} // namespace PythonType

#endif // PYTHON_SEQUENCE_CONVERTER_HPP

