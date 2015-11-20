#pragma once
#ifndef _PYTHON_MAPPING_COLLECTION_HPP
#define _PYTHON_MAPPING_COLLECTION_HPP


#include "core_variant/collection.hpp"

#include "mapping_iterator.hpp"

#include "core_script/type_converter_queue.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <type_traits>


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace PythonType
{


/**
 *	Wraps a PyScript::ScriptDict with the Collection system's interface.
 *	Unordered map.
 */
class Mapping final : public CollectionImplBase
{
public:
	typedef Mapping base;
	typedef PyScript::ScriptDict container_type;
	typedef PyScript::ScriptObject key_type;
	typedef PyScript::ScriptObject value_type;
	typedef Mapping this_type;

	typedef MappingIterator iterator_impl_type;

	Mapping( const container_type & container,
		const PythonTypeConverters & typeConverters );

	virtual bool empty() const override;
	virtual size_t size() const override;

	virtual CollectionIteratorImplPtr begin() override;
	virtual CollectionIteratorImplPtr end() override;

	virtual std::pair< CollectionIteratorImplPtr, bool > get(
		const Variant & key,
		CollectionImplBase::GetPolicy policy ) override;

	virtual CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr & pos ) override;
	virtual size_t erase( const Variant & key ) override;
	virtual CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr & first,
		const CollectionIteratorImplPtr& last ) override;

	virtual const TypeId & keyType() const override;
	virtual const TypeId & valueType() const override;
	virtual const TypeId & containerType() const override;
	virtual void * containerData() const override;
	virtual bool isMapping() const override;
	virtual bool canResize() const override;

private:
	container_type container_;
	const PythonTypeConverters & typeConverters_;
};


} // namespace PythonType


#endif // _PYTHON_MAPPING_COLLECTION_HPP
