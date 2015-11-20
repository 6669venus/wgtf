#pragma once
#ifndef _PYTHON_MAPPING_ITERATOR_HPP
#define _PYTHON_MAPPING_ITERATOR_HPP


#include "core_variant/collection.hpp"

#include "core_script/type_converter_queue.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <type_traits>


namespace PythonType
{
class IConverter;
} // namespace PythonType


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace PythonType
{


/**
 *	This class wraps a PyScript::ScriptDict
 *	with the Collection system's interface.
 */
class MappingIterator final : public CollectionIteratorImplBase
{
public:
	typedef PyScript::ScriptDict container_type;
	typedef PyScript::ScriptObject key_type;
	typedef PyScript::ScriptObject value_type;
	typedef MappingIterator this_type;

	MappingIterator( const container_type & container,
		const PyScript::ScriptList::size_type index,
		const PythonTypeConverters & typeConverters );
	MappingIterator( const container_type & container,
		const key_type & key,
		const PythonTypeConverters & typeConverters );

	const container_type & container() const;
	PyScript::ScriptList::size_type index() const;
	virtual Variant key() const override;
	virtual Variant value() const override;
	virtual bool setValue( const Variant & value ) const override;
	virtual void inc() override;
	virtual bool equals( const CollectionIteratorImplBase & that ) const override;
	virtual bool lessThan( const CollectionIteratorImplBase & that ) const override;
	virtual CollectionIteratorImplPtr clone() const override;

private:
	container_type container_;
	PyScript::ScriptList keys_;
	PyScript::ScriptList::size_type index_;
	key_type key_;
	const PythonTypeConverters & typeConverters_;
};


} // namespace PythonType


#endif // _PYTHON_MAPPING_ITERATOR_HPP
