#include "pch.hpp"
#include "mapping_collection.hpp"

#include "i_type_converter.hpp"

#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"

namespace PythonType
{


namespace Detail
{


std::pair< CollectionIteratorImplPtr, bool > insert(
	Mapping::container_type & container_,
	const Mapping::key_type key,
	CollectionIteratorImplPtr end,
	const PythonTypeConverters & typeConverters_ )
{
	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

	auto noneType = PyScript::ScriptObject( Py_None,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	const bool success = container_.setItem( key,
		noneType,
		PyScript::ScriptErrorPrint() );
	if (success)
	{
		return result_type(
			std::make_shared< Mapping::iterator_impl_type >(
				container_,
				key,
				typeConverters_ ),
			true );
	}
	return result_type( end, false );
}


} // namespace Detail


Mapping::Mapping( const Mapping::container_type & container,
	const PythonTypeConverters & typeConverters )
	: CollectionImplBase()
	, container_( container )
	, typeConverters_( typeConverters )
{
}


bool Mapping::empty() const /* override */
{
	return (container_.size() == 0);
}


size_t Mapping::size() const /* override */
{
	return container_.size();
}


CollectionIteratorImplPtr Mapping::begin() /* override */
{
	// Start index into dict.keys()
	const PyScript::ScriptList::size_type startIndex = 0;
	return std::make_shared< iterator_impl_type >( container_,
		startIndex,
		typeConverters_ );
}


CollectionIteratorImplPtr Mapping::end() /* override */
{
	return std::make_shared< iterator_impl_type >( container_,
		PyScript::ScriptObject( nullptr ),
		typeConverters_ );
}


std::pair< CollectionIteratorImplPtr, bool > Mapping::get(
	const Variant & variantKey,
	CollectionImplBase::GetPolicy policy ) /* override */
{
	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

	PyScript::ScriptObject scriptKey;
	const bool success = typeConverters_.toScriptType( variantKey, scriptKey );
	if (!success)
	{
		return result_type( this->end(), false );
	}

	if (policy == GET_EXISTING)
	{
		return result_type(
			std::make_shared< iterator_impl_type >( container_,
				scriptKey,
				typeConverters_ ),
			false );
	}
	else if (policy == GET_NEW)
	{
		return Detail::insert( container_, scriptKey, this->end(), typeConverters_ );
	}
	else if (policy == GET_AUTO)
	{
		auto scriptValue = container_.getItem( scriptKey,
			PyScript::ScriptErrorPrint() );
		if (scriptValue.exists())
		{
			// Get existing
			return result_type(
				std::make_shared< iterator_impl_type >( container_,
					scriptKey,
					typeConverters_ ),
				false );
		}

		// Insert new at start or end
		return Detail::insert( container_, scriptKey, this->end(), typeConverters_ );
	}
	else
	{
		assert( false && "Not implemented" );
		return result_type( this->end(), false );
	}
}


CollectionIteratorImplPtr Mapping::erase(
	const CollectionIteratorImplPtr & pos ) /* override */
{
	return nullptr;
}


size_t Mapping::erase( const Variant & key ) /* override */
{
	return 0;
}


CollectionIteratorImplPtr Mapping::erase( const CollectionIteratorImplPtr & first,
	const CollectionIteratorImplPtr& last ) /* override */
{
	return nullptr;
}


const TypeId & Mapping::keyType() const /* override */
{
	static auto s_KeyType = TypeId::getType< key_type >();
	return s_KeyType;
}


const TypeId & Mapping::valueType() const /* override */
{
	static auto s_ValueType = TypeId::getType< value_type >();
	return s_ValueType;
}


const TypeId & Mapping::containerType() const /* override */
{
	return TypeId::getType< container_type >();
}


void * Mapping::containerData() const /* override */
{
	return const_cast< void * >( static_cast< const void * >( &container_ ) );
}


bool Mapping::isMapping() const /* override */
{
	return true;
}


bool Mapping::canResize() const /* override */
{
	return true;
}


} // namespace PythonType
