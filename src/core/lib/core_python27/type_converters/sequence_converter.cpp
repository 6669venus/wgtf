#include "pch.hpp"

#include "sequence_converter.hpp"

#include "core_variant/collection.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


namespace Script
{
PyObject * getData( const Collection& data )
{
	return nullptr;
}

int setData( PyObject * pObj, 
	std::vector< Variant > & rVal, const char * varName = "" )
{
	if (!PyList_Check( pObj ))
	{
		PyErr_Format( PyExc_TypeError,
			"%s must be set to a list", varName );
		return -1;
	}

	PyScript::ScriptList scriptList( pObj,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	const auto size = scriptList.size();
	rVal.reserve( size );
	for (PyScript::ScriptList::size_type i = 0; i < size; ++i)
	{
		const auto scriptItem = scriptList.getItem( i );
		rVal.emplace_back( Variant(
			scriptItem.str( PyScript::ScriptErrorPrint() ).c_str() ) );
	}

	return 0;
}


} // namespace Scriot
namespace PythonType
{


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

	const int ret = Script::setData( inObject.get(), children, "" );
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

	//auto result = PyScript::ScriptObject::createFrom( value );
	//if (!result.exists())
	//{
		return false;
	//}

	//outObject = result;
	//return true;
}


} // namespace PythonType
