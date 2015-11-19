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
	, key_()
	, typeConverters_( typeConverters )
{
	// Only supports positive indices
	assert( index_ > 0 );
	if (index_ < container_.size())
	{
		auto scriptKey = keys_.getItem( index_ );
		
		PyScript::ScriptString str = scriptKey.str( PyScript::ScriptErrorPrint() );
		const char * value = str.c_str();

		key_ = value;
	}
}


MappingIterator::MappingIterator( const container_type & container,
	const key_type & key,
	const PythonTypeConverters & typeConverters )
	: container_( container )
	, keys_( container_.keys( PyScript::ScriptErrorPrint() ) )
	, index_( 0 )
	, key_()
	, typeConverters_( typeConverters )
{
	for (; index_ < keys_.size(); ++index_)
	{
		auto scriptKey = keys_.getItem( index_ );
		
		PyScript::ScriptString str = scriptKey.str( PyScript::ScriptErrorPrint() );
		const char * value = str.c_str();

		if (strncmp( key.c_str(), value, key.size() ) == 0)
		{
			key_ = key;
			break;
		}
	}
}


const MappingIterator::container_type & MappingIterator::container() const
{
	return container_;
}


PyScript::ScriptList::size_type MappingIterator::index() const
{
	return index_;
}


Variant MappingIterator::key() const /* override */
{
	return Variant( key_ );
}


Variant MappingIterator::value() const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "IndexError: mapping index out of range\n" );
		return Variant();
	}

	PyScript::ScriptObject item = container_.getItem( key_.c_str(),
		PyScript::ScriptErrorPrint() );
	
	Variant result;
	const bool success = typeConverters_.toVariant( item, result );
	return result;
}


bool MappingIterator::setValue( const Variant & value ) const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "IndexError: mapping assignment index out of range\n" );
		return false;
	}

	PyScript::ScriptObject scriptValue;
	const bool success = typeConverters_.toScriptType( value, scriptValue );
	if (!success)
	{
		return false;
	}

	return container_.setItem( key_.c_str(),
		scriptValue,
		PyScript::ScriptErrorPrint() );
}


void MappingIterator::inc() /* override */
{
	++index_;
	auto scriptKey = keys_.getItem( index_ );
	
	PyScript::ScriptString str = scriptKey.str( PyScript::ScriptErrorPrint() );
	const char * value = str.c_str();

	key_ = value;
}


bool MappingIterator::equals(
	const CollectionIteratorImplBase & that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	if (!t)
	{
		return false;
	}

	return (container_ == t->container_) &&
		(index_ == t->index_) &&
		(key_ == t->key_);
}


bool MappingIterator::lessThan(
	const CollectionIteratorImplBase & that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	assert(t);
	if (!t)
	{
		return false;
	}
	assert( container_ == t->container_ );
	return (index_ < t->index_);
}


CollectionIteratorImplPtr MappingIterator::clone() const /* override */
{
	return std::make_shared< this_type >( *this );
}


} // namespace PythonType
