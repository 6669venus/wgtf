#pragma once
#ifndef PYTHON_SEQUENCE_CONVERTER_HPP
#define PYTHON_SEQUENCE_CONVERTER_HPP

#include "i_type_converter.hpp"
#include "core_script/type_converter_queue.hpp"


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace PythonType
{


/**
 *	Attempts to convert ScriptObject<->sequence<->Variant.
 */
class SequenceConverter : public IConverter
{
public:
	SequenceConverter( const PythonTypeConverters & typeConverters );

	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
private:
	const PythonTypeConverters & typeConverters_;
};


} // namespace PythonType

#endif // PYTHON_SEQUENCE_CONVERTER_HPP

