#pragma once
#ifndef PYTHON_DEFINITION_HPP
#define PYTHON_DEFINITION_HPP


#include "core_reflection/interfaces/i_class_definition_details.hpp"


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
{
public:
	DefinitionDetails( IComponentContext & context,
		const PyScript::ScriptObject & pythonObject );

	void init( IClassDefinitionModifier & collection ) override;
	bool isAbstract() const override;
	bool isGeneric() const override;
	const char * getName() const override;
	const char * getParentName() const override;
	const MetaBase * getMetaData() const override;
	ObjectHandle createBaseProvider( const ReflectedPolyStruct & ) const override;
	ObjectHandle createBaseProvider( const IClassDefinition & classDefinition,
		const void * pThis ) const override;
	ObjectHandle create( const IClassDefinition & classDefinition ) const override;
	CastHelperCache * getCastHelperCache() const override;
	void * upCast( void * object ) const override;

	static std::string generateName( const PyScript::ScriptObject & object );

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;
};


} // namespace ReflectedPython


#endif // PYTHON_DEFINITION_HPP
