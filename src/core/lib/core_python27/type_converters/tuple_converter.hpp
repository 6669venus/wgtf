#pragma once
#ifndef _PYTHON_TUPLE_CONVERTER_HPP
#define _PYTHON_TUPLE_CONVERTER_HPP

#include "i_python_converter.hpp"

class IComponentContext;

namespace PythonType
{

class Converters;


/**
 *	Attempts to convert ScriptTuple<->Collection<->Variant.
 */
class TupleConverter final : public IPythonConverter
{
public:
	TupleConverter( IComponentContext & context,
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

#endif // _PYTHON_TUPLE_CONVERTER_HPP

