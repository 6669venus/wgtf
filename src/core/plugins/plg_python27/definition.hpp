#pragma once
#ifndef PYTHON_DEFINITION_HPP
#define PYTHON_DEFINITION_HPP


#include "core_reflection/interfaces/i_class_definition.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include "property.hpp"


/**
 *	Implements the IClassDefinition interface for Python objects.
 *	At the moment it's just a wrapper for a generic definition.
 */
class Definition : public IClassDefinition
{
public:
	Definition::Definition( IDefinitionManager & definitionManager,
		PyScript::ScriptObject& pythonObject );

	const IClassDefinitionDetails & getDetails() const override;
	PropertyAccessor bindProperty( const char * name,
		const ObjectHandle & object ) const override;

	void bindPropertyImpl( const char * name,
		const ObjectHandle & pBase,
		PropertyAccessor & o_PropertyAccessor ) const override;

	IDefinitionManager * getDefinitionManager() const override;

	bool isGeneric() const override;
	IClassDefinition * getParent() const override;
	bool canBeCastTo( const IClassDefinition & definition ) const override;
	void * castTo( const IClassDefinition & definition,
		void * object ) const override;
	void * upCast( void * object ) const override;

	PropertyIteratorRange allProperties() const override;
	PropertyIteratorRange directProperties() const override;
	const SortedPropertyCollection & sortedProperties() const override;
	PropertyIterator getPropertyIterator(
		PropertyIterator::IterateStrategy strategy
			= PropertyIterator::ITERATE_PARENTS ) const override;
	IBaseProperty * findProperty( const TypeId & propertyId ) const override;

	const char * getName() const override;
	const MetaBase * getMetaData() const override;
	ObjectHandle create() const override;
	ObjectHandle createManagedObject(
		const RefObjectId & id = RefObjectId::zero() ) const override;

	ObjectHandle getBaseProvider(
		const ReflectedPolyStruct * polyStruct ) const override;
	ObjectHandle getBaseProvider( const void * pThis ) const override;

private:
	IClassDefinition* pGenericDefinition_;
	typedef std::vector< Property > AttributeProperties;
	AttributeProperties attributes_;
};


#endif // PYTHON_DEFINITION_HPP
