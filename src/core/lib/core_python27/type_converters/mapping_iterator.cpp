#include "pch.hpp"

#include "mapping_iterator.hpp"

#include "i_type_converter.hpp"

#include "core_variant/variant.hpp"

#include <cassert>


namespace PythonType
{


MappingIterator::MappingIterator( const container_type & container,
	const PyScript::ScriptList::size_type index,
	const PythonTypeConverters & typeConverters )
	: container_( container )
	, keys_( container_.keys( PyScript::ScriptErrorPrint() ) )
	, index_( index )
	, key_( nullptr )
	, typeConverters_( typeConverters )
{
	// Only supports positive indices
	assert( index_ >= 0 );
	if (index_ < container_.size())
	{
		key_ = keys_.getItem( index_ );
	}
}


MappingIterator::MappingIterator( const container_type & container,
	const key_type & key,
	const PythonTypeConverters & typeConverters )
	: container_( container )
	, keys_( container_.keys( PyScript::ScriptErrorPrint() ) )
	, index_( 0 )
	, key_( key )
	, typeConverters_( typeConverters )
{
	if (!key_.exists())
	{
		index_ = container_.size();
	}
	else
	{
		// If the key is not found, then index_ == end
		for (; index_ < keys_.size(); ++index_)
		{
			auto scriptKey = keys_.getItem( index_ );
			if (key == scriptKey)
			{
				break;
			}
		}
	}
}


const MappingIterator::container_type & MappingIterator::container() const
{
	return container_;
}


MappingIterator::key_type MappingIterator::rawKey() const
{
	return key_;
}


PyScript::ScriptList::size_type MappingIterator::rawIndex() const
{
	return index_;
}


Variant MappingIterator::key() const /* override */
{
	Variant result;
	const bool success = typeConverters_.toVariant( key_, result );
	assert( success );
	return result;
}


Variant MappingIterator::value() const /* override */
{
	if (!key_.exists() || (index_ < 0) || (index_ >= container_.size()))
	{

		// HACK Try cast key to index

		Variant result;
		const bool success = typeConverters_.toVariant( key_, result );
		PyScript::ScriptList::size_type fakeIndex = container_.size();
		result.tryCast( fakeIndex );

		if ((fakeIndex < 0) || (fakeIndex >= container_.size()))
		{
			NGT_ERROR_MSG( "KeyError: %s\n",
				key_.str( PyScript::ScriptErrorPrint() ).c_str() );
			return Variant();
		}

		key_ = keys_.getItem( fakeIndex );
		index_ = fakeIndex;
	}

	PyScript::ScriptObject item = container_.getItem( key_,
		PyScript::ScriptErrorPrint() );
	
	Variant result;
	const bool success = typeConverters_.toVariant( item, result );
	assert( success );
	return result;
}


bool MappingIterator::setValue( const Variant & value ) const /* override */
{
	if (!key_.exists() || (index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "KeyError: %s\n",
			key_.str( PyScript::ScriptErrorPrint() ).c_str() );
		return false;
	}

	PyScript::ScriptObject scriptValue;
	const bool success = typeConverters_.toScriptType( value, scriptValue );
	if (!success)
	{
		return false;
	}

	return container_.setItem( key_,
		scriptValue,
		PyScript::ScriptErrorPrint() );
}


void MappingIterator::inc() /* override */
{
	++index_;
	if (index_ >= container_.size())
	{
		key_ = nullptr;
		return;
	}

	auto scriptKey = keys_.getItem( index_ );
	key_ = scriptKey;
}


bool MappingIterator::equals(
	const CollectionIteratorImplBase & that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	assert( t );
	if (!t)
	{
		return false;
	}

	assert( container_ == t->container_ );
	return (index_ == t->index_);
}


CollectionIteratorImplPtr MappingIterator::clone() const /* override */
{
	return std::make_shared< this_type >( *this );
}


} // namespace PythonType
