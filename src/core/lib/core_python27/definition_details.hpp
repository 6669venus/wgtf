#pragma once
#ifndef PYTHON_DEFINITION_HPP
#define PYTHON_DEFINITION_HPP


#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"

#include <memory>
#include <string>


class IComponentContext;
namespace PyScript
{
	class ScriptObject;
} // namespace PyScript


namespace ReflectedPython
{


/**
 *	Implements the IClassDefinitionDetails interface for Python objects.
 */
class DefinitionDetails
	: public IClassDefinitionDetails
	, public IClassDefinitionModifier
{
public:
	DefinitionDetails( IComponentContext & context,
		const PyScript::ScriptObject & pythonObject );

	bool isAbstract() const override;
	bool isGeneric() const override;
	const char * getName() const override;
	const char * getParentName() const override;
	MetaHandle getMetaData() const override;
	ObjectHandle create( const IClassDefinition & classDefinition ) const override;
	void * upCast( void * object ) const override;
	std::shared_ptr< PropertyIteratorImplBase > getPropertyIterator() const override;
	IClassDefinitionModifier * getDefinitionModifier() const override;

	void addProperty( const IBasePropertyPtr & reflectedProperty, MetaHandle metaData ) override;

	static std::string generateName( const PyScript::ScriptObject & object );

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;
};


} // namespace ReflectedPython


#endif // PYTHON_DEFINITION_HPP
