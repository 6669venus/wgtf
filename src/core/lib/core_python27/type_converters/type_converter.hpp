#pragma once
#ifndef PYTHON_TYPE_CONVERTER_HPP
#define PYTHON_TYPE_CONVERTER_HPP

#include "i_type_converter.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"


namespace PythonType
{
	/**
	 *	Attempts to convert ScriptObject<->Variant.
	 */
	class TypeConverter : public IConverter
	{
	public:
		TypeConverter( IComponentContext & context );


		virtual bool toVariant( const PyScript::ScriptObject & inObject, Variant & outVariant ) override;


		virtual bool toScriptType( const Variant & inVariant, PyScript::ScriptObject & outObject ) override;


	private:
		IComponentContext & context_;
	};
} // namespace PythonType


#endif // PYTHON_TYPE_CONVERTER_HPP
