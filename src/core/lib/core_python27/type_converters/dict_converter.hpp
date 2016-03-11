#pragma once
#ifndef _PYTHON_DICT_CONVERTER_HPP
#define _PYTHON_DICT_CONVERTER_HPP

#include "converters.hpp"
#include "i_python_converter.hpp"

class IComponentContext;

namespace PythonType
{


/**
 *	Attempts to convert ScriptDict<->Collection<->Variant.
 */
class DictConverter final : public IPythonConverter
{
public:
	DictConverter( IComponentContext & context,
		const Converters & typeConverters );

	virtual bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant,
		const ObjectHandle & parentHandle,
		const std::string & childPath ) override;
	virtual bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) override;
private:
	IComponentContext & context_;
	const Converters & typeConverters_;
};


} // namespace PythonType

#endif // _PYTHON_DICT_CONVERTER_HPP

