#pragma once
#ifndef PYTHON_LIST_COLLECTION_HPP
#define PYTHON_LIST_COLLECTION_HPP


#include "core_variant/collection.hpp"

#include "list_iterator.hpp"

#include "core_script/type_converter_queue.hpp"
#include "wg_pyscript/py_script_object.hpp"


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace PythonType
{


class List final : public CollectionImplBase
{
public:
	typedef List base;
	typedef PyScript::ScriptList container_type;
	typedef container_type::size_type key_type;
	typedef Variant value_type;
	typedef List this_type;

	typedef ListIteratorImpl iterator_impl_type;

	List( const container_type & container,
		const PythonTypeConverters & typeConverters );

	virtual bool empty() const override;
	virtual size_t size() const override;

	virtual CollectionIteratorImplPtr begin() override;
	virtual CollectionIteratorImplPtr end() override;

	virtual std::pair< CollectionIteratorImplPtr, bool > get( const Variant & key,
		CollectionImplBase::GetPolicy policy ) override;

	virtual CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr & pos ) override;
	virtual size_t erase( const Variant & key ) override;
	virtual CollectionIteratorImplPtr erase( const CollectionIteratorImplPtr & first,
		const CollectionIteratorImplPtr& last ) override;

	virtual const TypeId & keyType() const override;
	virtual const TypeId & valueType() const override;

	virtual const TypeId & containerType() const override;
	virtual void * containerData() const override;

private:
	container_type container_;
	const PythonTypeConverters & typeConverters_;
};


} // namespace PythonType


#endif // PYTHON_LIST_COLLECTION_HPP
