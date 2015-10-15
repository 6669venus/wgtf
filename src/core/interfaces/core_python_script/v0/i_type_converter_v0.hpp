#pragma once
#ifndef I_V0_PYTHON_TYPE_CONVERTER_V0_HPP
#define I_V0_PYTHON_TYPE_CONVERTER_V0_HPP

#include "core_dependency_system/i_interface.hpp"

namespace PyScript
{
	class ScriptObject;
} // namespace PyScript

class Variant;

DECLARE_INTERFACE_BEGIN( IPythonTypeConverter, 0, 0 )

	
	/**
	 *	Convert the given ScriptObject to a Variant.
	 *	
	 *	@param inObject the object to be converted.
	 *	@param outVariant storage for the converted item.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	virtual bool toVariant( const PyScript::ScriptObject& inObject,
		Variant& outVariant ) = 0;


	/**
	 *	Convert the given Variant to a ScriptObject.
	 *	
	 *	@param outVariant the object to be converted.
	 *	@param outObject storage for the converted item.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	virtual bool toScriptObject( const Variant& inVariant,
		PyScript::ScriptObject& outObject ) = 0;


DECLARE_INTERFACE_END()


#endif // I_V0_PYTHON_TYPE_CONVERTER_V0_HPP

