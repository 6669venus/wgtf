#pragma once
#ifndef PYTHON_DEFINITION_HPP
#define PYTHON_DEFINITION_HPP


#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include "property.hpp"


class IPythonTypeConverter;
template < typename ITypeConverter, typename ScriptType >
class TypeConverterQueue;
typedef TypeConverterQueue< IPythonTypeConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace ReflectedPython
{


/**
 *	Implements the IClassDefinitionDetails interface for Python objects.
 */
class DefinitionDetails : public IClassDefinitionDetails
{
public:
	DefinitionDetails::DefinitionDetails( IDefinitionManager & definitionManager,
		PyScript::ScriptObject & pythonObject,
		const PythonTypeConverters & typeConverters );

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

private:
	const PythonTypeConverters & typeConverters_;
	std::string name_;
	PyScript::ScriptObject pythonObject_;

	std::unique_ptr< const MetaBase > metaData_;
	mutable CastHelperCache castHelperCache_;
};


} // namespace ReflectedPython


#endif // PYTHON_DEFINITION_HPP
