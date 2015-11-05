#include "pch.hpp"

#include "sequence_converter.hpp"

#include "core_variant/collection.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


namespace Script
{
PyObject * getData( const Collection & data,
	const PythonTypeConverters & typeConverters )
{
	const auto size = data.size();
	auto scriptList = PyScript::ScriptList::create( size );

	auto itr = data.cbegin();
	for (size_t i = 0; i < size; ++i)
	{
		const auto variantItem = (*itr);
		PyScript::ScriptObject scriptItem;
		const bool convertResult = typeConverters.toScriptType(
			variantItem, scriptItem );
		if (!convertResult)
		{
			PyErr_Format( PyExc_TypeError,
				"Could not get item %d", i );
			return nullptr;
		}

		const bool setResult = scriptList.setItem(
			static_cast< PyScript::ScriptList::size_type >( i ),
			scriptItem );
		if (!setResult)
		{
			PyErr_Format( PyExc_TypeError,
				"Could not set item %d", i );
			return nullptr;
		}

		++itr;
	}

	PyObject * pyObject = scriptList.get();
	Py_INCREF( pyObject );
	return pyObject;
}

int setData( PyObject * pObj, 
	std::vector< Variant > & rVal,
	const PythonTypeConverters & typeConverters )
{
	PyScript::ScriptList scriptList( pObj,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	const auto size = scriptList.size();
	rVal.reserve( size );
	for (PyScript::ScriptList::size_type i = 0; i < size; ++i)
	{
		const auto scriptItem = scriptList.getItem( i );
		Variant variantItem;
		const bool result = typeConverters.toVariant( scriptItem, variantItem );
		if (!result)
		{
			PyErr_Format( PyExc_TypeError,
				"Could not get item %d", i );
			return -1;
		}

		rVal.emplace_back( variantItem );
	}

	return 0;
}


} // namespace Scriot
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

	typedef std::vector< Variant > Children;
	auto collectionHolder =
		std::make_shared< CollectionHolder< Children > >();
	Children& children = collectionHolder->storage();
	PyScript::ScriptErrorClear errorHandler;
	//const bool result = inObject.convertTo( value, PyScript::ScriptErrorClear() );

	const int ret = Script::setData( inObject.get(), children, typeConverters_ );
	errorHandler.checkMinusOne( ret );
	const bool result = (ret == 0);

	if (!result)
	{
		return false;
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


	auto result = Script::getData( value, typeConverters_ );
	if (result == nullptr)
	{
		return false;
	}

	outObject = result;
	Py_DECREF( result );
	return true;
}


} // namespace PythonType
