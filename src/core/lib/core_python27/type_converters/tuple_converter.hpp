#pragma once
#ifndef _PYTHON_TUPLE_CONVERTER_HPP
#define _PYTHON_TUPLE_CONVERTER_HPP

#include "i_type_converter.hpp"
#include "core_script/type_converter_queue.hpp"


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace PythonType
{


/**
 *	Attempts to convert ScriptTuple<->Collection<->Variant.
 */
class TupleConverter final : public IConverter
{
public:
	TupleConverter( const PythonTypeConverters & typeConverters );

	virtual bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	virtual bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
private:
	const PythonTypeConverters & typeConverters_;
};


} // namespace PythonType

#endif // _PYTHON_TUPLE_CONVERTER_HPP

