#pragma once
#ifndef _PYTHON_DICT_CONVERTER_HPP
#define _PYTHON_DICT_CONVERTER_HPP

#include "i_type_converter.hpp"
#include "core_script/type_converter_queue.hpp"


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace PythonType
{


/**
 *	Attempts to convert ScriptDict<->Collection<->Variant.
 */
class DictConverter final : public IConverter
{
public:
	DictConverter( const PythonTypeConverters & typeConverters );

	virtual bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant,
		void * parent,
		const std::string & path ) override;
	virtual bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
private:
	const PythonTypeConverters & typeConverters_;
};


} // namespace PythonType

#endif // _PYTHON_DICT_CONVERTER_HPP

