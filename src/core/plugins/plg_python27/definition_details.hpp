#pragma once
#ifndef PYTHON_DEFINITION_HPP
#define PYTHON_DEFINITION_HPP


#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include "property.hpp"


/**
 *	Implements the IClassDefinitionDetails interface for Python objects.
 */
class DefinitionDetails : public IClassDefinitionDetails
{
public:
	DefinitionDetails::DefinitionDetails( IDefinitionManager & definitionManager,
		PyScript::ScriptObject& pythonObject );

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
	PyScript::ScriptObject pythonObject_;

	typedef std::vector< Property > AttributeProperties;
	AttributeProperties attributes_;

	std::unique_ptr< const MetaBase > metaData_;
	mutable CastHelperCache castHelperCache_;
};


#endif // PYTHON_DEFINITION_HPP
