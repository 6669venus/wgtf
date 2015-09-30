#include "pch.hpp"
#include "definition.hpp"
#include "core_reflection/property_accessor.hpp"

namespace
{

/**
 *	Get attributes from the Python object and add them to the definition.
 */
void extractAttributes( PyScript::ScriptObject& pythonObject,
	IClassDefinition& definition,
	std::vector< Property >& properties )
{
	if (pythonObject.get() == nullptr)
	{
		return;
	}

	// Get a list of strings appropriate for object arguments
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptObject dir = pythonObject.getDir( errorHandler );
	if (dir.get() == nullptr)
	{
		return;
	}
	PyScript::ScriptIter iter = dir.getIter( errorHandler );
	if (iter.get() == nullptr)
	{
		return;
	}

	while (PyScript::ScriptObject item = iter.next())
	{
		// Add property to definition
		// TODO NGT-1051 only adding name for now
		PyScript::ScriptString str = item.str( errorHandler );
		const char* name = str.c_str();

		properties.emplace_back( Property( name ) );
	}
}

} // namespace

Definition::Definition( IDefinitionManager & definitionManager,
	PyScript::ScriptObject& pythonObject )
	: pGenericDefinition_( nullptr )
{
	PyScript::ScriptErrorPrint errorHandler;

	// Note: this will make a unique class definition name per instance,
	// not per type
	PyScript::ScriptString scriptString = pythonObject.str( errorHandler );
	const char* classDefinitionName = scriptString.c_str();

	auto definitionDetails = 
		definitionManager.createGenericDefinition( classDefinitionName );
	pGenericDefinition_ =
		definitionManager.registerDefinition( definitionDetails );
	assert( pGenericDefinition_ != nullptr );

	extractAttributes( pythonObject, (*pGenericDefinition_), attributes_ );
}

const IClassDefinitionDetails & Definition::getDetails() const
{
	return pGenericDefinition_->getDetails();
}

PropertyAccessor Definition::bindProperty( const char * name,
	const ObjectHandle & object ) const
{
	return pGenericDefinition_->bindProperty();
}

void Definition::bindPropertyImpl( const char * name,
	const ObjectHandle & pBase,
	PropertyAccessor & o_PropertyAccessor ) const
{
	return pGenericDefinition_->bindPropertyImpl(
		name, pBase, o_PropertyAccessor );
}

IDefinitionManager * Definition::getDefinitionManager() const
{
	return pGenericDefinition_->getDefinitionManager();
}

bool Definition::isGeneric() const
{
	return pGenericDefinition_->isGeneric();
}

IClassDefinition * Definition::getParent() const
{
	return pGenericDefinition_->getParent();
}

bool Definition::canBeCastTo( const IClassDefinition & definition ) const
{
	return pGenericDefinition_->canBeCastTo( definition );
}

void * Definition::castTo( const IClassDefinition & definition,
	void * object ) const
{
	return pGenericDefinition_->castTo( definition, object );
}

void * Definition::upCast( void * object ) const
{
	return pGenericDefinition_->upCast( object );
}

PropertyIteratorRange Definition::allProperties() const
{
	return pGenericDefinition_->allProperties();
}

PropertyIteratorRange Definition::directProperties() const
{
	return pGenericDefinition_->directProperties();
}

const SortedPropertyCollection & Definition::sortedProperties() const
{
	return pGenericDefinition_->sortedProperties();
}

PropertyIterator Definition::getPropertyIterator(
	PropertyIterator::IterateStrategy strategy ) const
{
	return pGenericDefinition_->getPropertyIterator( strategy );
}

IBaseProperty * Definition::findProperty( const TypeId & propertyId ) const
{
	return pGenericDefinition_->findProperty( propertyId );
}

const char * Definition::getName() const
{
	return pGenericDefinition_->getName();
}

const MetaBase * Definition::getMetaData() const
{
	return pGenericDefinition_->getMetaData();
}

ObjectHandle Definition::create() const
{
	return pGenericDefinition_->create();
}

ObjectHandle Definition::createManagedObject(
	const RefObjectId & id /*= RefObjectId::zero()*/ ) const
{
	return pGenericDefinition_->createManagedObject( id );
}

ObjectHandle Definition::getBaseProvider(
	const ReflectedPolyStruct * polyStruct ) const
{
	return pGenericDefinition_->getBaseProvider( polyStruct );
}

ObjectHandle Definition::getBaseProvider( const void * pThis ) const
{
	return pGenericDefinition_->getBaseProvider( pThis );
}
