#include "pch.hpp"

#include "sequence_converter.hpp"

#include "core_variant/collection.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


namespace PythonType
{


SequenceConverter::SequenceConverter( const PythonTypeConverters & typeConverters )
	: IConverter()
	, typeConverters_( typeConverters )
{
}


bool SequenceConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	if (!PyScript::ScriptList::check( inObject ))
	{
		return false;
	}
	PyScript::ScriptList scriptList( inObject.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	typedef std::vector< Variant > Children;
	auto collectionHolder =
		std::make_shared< CollectionHolder< Children > >();
	Children& children = collectionHolder->storage();

	const auto size = scriptList.size();
	children.reserve( static_cast< Children::size_type >( size ) );
	for (PyScript::ScriptList::size_type i = 0; i < size; ++i)
	{
		const auto scriptItem = scriptList.getItem( i );
		Variant variantItem;
		const bool convertResult = typeConverters_.toVariant(
			scriptItem, variantItem );
		if (!convertResult)
		{
			return false;
		}

		children.emplace_back( variantItem );
	}

	Collection childrenCollection( collectionHolder );
	outVariant = Variant( childrenCollection );
	return true;
}


bool SequenceConverter::toScriptType( const Variant & inVariant,
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

	const auto size = static_cast< PyScript::ScriptList::size_type >( value.size() );
	auto scriptList = PyScript::ScriptList::create( size );

	auto itr = value.cbegin();
	for (PyScript::ScriptList::size_type i = 0; i < size; ++i)
	{
		const auto variantItem = (*itr);
		PyScript::ScriptObject scriptItem;
		const bool convertResult = typeConverters_.toScriptType(
			variantItem, scriptItem );
		if (!convertResult)
		{
			return false;
		}

		const bool setResult = scriptList.setItem( i, scriptItem );
		if (!setResult)
		{
			return false;
		}

		++itr;
	}

	outObject = scriptList;
	return true;
}


} // namespace PythonType
