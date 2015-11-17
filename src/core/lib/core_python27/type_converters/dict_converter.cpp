#include "pch.hpp"

#include "dict_converter.hpp"

#include "mapping_collection.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


DictConverter::DictConverter( const PythonTypeConverters & typeConverters )
	: IConverter()
	, typeConverters_( typeConverters )
{
}


bool DictConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	if (!PyScript::ScriptDict::check( inObject ))
	{
		return false;
	}
	PyScript::ScriptDict ScriptDict( inObject.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	auto collectionHolder = std::make_shared< Mapping >(
		ScriptDict,
		typeConverters_ );
	Collection collection( collectionHolder );
	outVariant = Variant( collection );
	return true;
}


bool DictConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< Collection >::storage_type >())
	{
		return false;
	}
	Collection value;
	const auto isCollection = inVariant.tryCast< Collection >( value );
	if (!isCollection)
	{
		return false;
	}
	if ((value.keyType() != TypeId::getType< Variant >()) &&
		(value.keyType() != TypeId::getType< std::string >()))
	{
		return false;
	}

	const auto size = static_cast< PyScript::ScriptDict::size_type >( value.size() );
	auto scriptDict = PyScript::ScriptDict::create( size );

	for (auto itr = value.cbegin(); itr != value.cend(); ++itr)
	{
		const auto variantItem = itr.value();
		PyScript::ScriptObject scriptItem;
		const bool convertResult = typeConverters_.toScriptType(
			variantItem, scriptItem );
		if (!convertResult)
		{
			return false;
		}

		const auto variantKey = itr.key();
		std::string key;
		variantKey.tryCast< std::string >( key );
		const bool setResult = scriptDict.setItem( key.c_str(),
			scriptItem,
			PyScript::ScriptErrorPrint() );
		if (!setResult)
		{
			return false;
		}
	}

	outObject = scriptDict;
	return true;
}


} // namespace PythonType
