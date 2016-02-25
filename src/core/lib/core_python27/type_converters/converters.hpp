#pragma once

#include "core_script/type_converter_queue.hpp"
#include "default_converter.hpp"
#include "i_type_converter.hpp"

#include <string>


namespace PyScript
{
class ScriptObject;
} // namespace PyScript
namespace ReflectedPython
{
class DefinedInstance;
} // namespace ReflectedPython
class Variant;


namespace PythonType
{

typedef TypeConverterQueue< IConverter,
	PyScript::ScriptObject > BasicTypeConverters;

class Converters
{
public:

	Converters( const BasicTypeConverters & basicTypeConverters,
		const DefaultConverter & defaultConverter );

	/**
	 *	Convert the given Variant into a ScriptType by searching through the
	 *	type converters.
	 *	
	 *	Search is performed from most-recently-added to first-added.
	 *	
	 *	@param inVariant the variant to be converted.
	 *	@param outObject storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject ) const;


	/**
	 *	Convert the given ScriptType into a Variant by searching through the
	 *	type converters.
	 *	
	 *	Search is performed from most-recently-added to first-added.
	 *	
	 *	@param inObject the ScriptType to be converted.
	 *	@param outVariant storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@param pParent parent of inObject.
	 *	@param childPath path of inObject.
	 *	@return true on success.
	 */
	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant,
		const ReflectedPython::DefinedInstance & parent,
		const std::string & childPath ) const;

	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) const;

private:
	const BasicTypeConverters & basicTypeConverters_;
	const DefaultConverter & defaultConverter_;
};


} // namespace PythonType

