#include "pch.hpp"
#include "mapping_collection.hpp"

#include "i_type_converter.hpp"

#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"

namespace PythonType
{


namespace Detail
{


 Mapping::result_type insert( Mapping::container_type & container_,
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


Mapping::result_type Mapping::get( const Variant & key,
	CollectionImplBase::GetPolicy policy ) /* override */
{
	PyScript::ScriptObject scriptKey;
	const bool success = typeConverters_.toScriptType( key, scriptKey );
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
	const auto pItr = dynamic_cast< iterator_impl_type * >( pos.get() );
	assert( pItr != nullptr );
	if (pItr == nullptr)
	{
		return this->end();
	}
	assert( container_ == pItr->container() );
	if (container_ != pItr->container())
	{
		return this->end();
	}
	assert( pItr->index() >= 0 );
	if ((pItr->index() < 0) || (pItr->index() >= container_.size()))
	{
		return this->end();
	}

	const bool removed = container_.delItem( pItr->keyType(),
		PyScript::ScriptErrorPrint() );
	// Container does not match iterators
	assert( removed );
	if (!removed)
	{
		return this->end();
	}

	return std::make_shared< iterator_impl_type >( container_,
		pItr->index(),
		typeConverters_ );
}


size_t Mapping::erase( const Variant & key ) /* override */
{
	PyScript::ScriptObject scriptKey;
	const bool success = typeConverters_.toScriptType( key, scriptKey );
	if (!success)
	{
		return 0;
	}

	const bool removed = container_.delItem( scriptKey, PyScript::ScriptErrorPrint() );
	if (!removed)
	{
		return 0;
	}

	return 1;
}


CollectionIteratorImplPtr Mapping::erase( const CollectionIteratorImplPtr & first,
	const CollectionIteratorImplPtr& last ) /* override */
{
	const auto pFirst = dynamic_cast< iterator_impl_type * >( first.get() );
	const auto pLast = dynamic_cast< iterator_impl_type * >( last.get() );
	assert( (pFirst != nullptr) && (pLast != nullptr) );
	if ((pFirst == nullptr) || (pLast == nullptr))
	{
		return this->end();
	}
	assert( (container_ == pFirst->container()) && (container_ == pLast->container()) );
	if ((container_ != pFirst->container()) || (container_ != pLast->container()))
	{
		return this->end();
	}
	assert( pFirst->index() >= 0 );
	if ((pFirst->index() < 0) || (pFirst->index() >= container_.size()))
	{
		return this->end();
	}
	assert( pLast->index() >= 0 );
	if ((pLast->index() < 0) || (pLast->index() >= container_.size()))
	{
		return this->end();
	}
	assert( pFirst->index() <= pLast->index() );
	if (pFirst->index() > pLast->index())
	{
		return this->end();
	}
	if (pFirst->index() == pLast->index())
	{
		return this->end();
	}

	// Collect keys before erasing
	std::vector< key_type > foundKeys;
	auto current = (*pFirst);
	for (; !current.equals( *pLast ); current.inc())
	{
		foundKeys.push_back( current.keyType() );
	}
	const auto lastIndex = current.index();

	for (const auto & key : foundKeys)
	{
		const bool removed = container_.delItem( key,
			PyScript::ScriptErrorPrint() );
		// Container does not match iterators
		// Container will be left in an inconsistent state
		assert( removed );
		if (!removed)
		{
			return this->end();
		}
	}

	return std::make_shared< iterator_impl_type >( container_,
		lastIndex,
		typeConverters_ );
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
