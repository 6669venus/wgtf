#pragma once
#ifndef PYTHON_LIST_CONVERTER_HPP
#define PYTHON_LIST_CONVERTER_HPP

#include "i_type_converter.hpp"
#include "converters.hpp"


namespace PythonType
{


/**
 *	Attempts to convert ScriptList<->Collection<->Variant.
 */
class ListConverter final : public IConverter
{
public:
	ListConverter( const Converters & typeConverters );

	virtual bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	virtual bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
private:
	const Converters & typeConverters_;
};


} // namespace PythonType

#endif // PYTHON_LIST_CONVERTER_HPP

