#pragma once
#ifndef _PYTHON_TUPLE_CONVERTER_HPP
#define _PYTHON_TUPLE_CONVERTER_HPP

#include "i_type_converter.hpp"
#include "converters.hpp"


namespace PythonType
{


/**
 *	Attempts to convert ScriptTuple<->Collection<->Variant.
 */
class TupleConverter final : public IConverter
{
public:
	TupleConverter( const Converters & typeConverters );

	virtual bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	virtual bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
private:
	const Converters & typeConverters_;
};


} // namespace PythonType

#endif // _PYTHON_TUPLE_CONVERTER_HPP

