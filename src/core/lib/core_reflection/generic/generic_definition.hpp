#ifndef GENERIC_DEFINITION_HPP
#define GENERIC_DEFINITION_HPP

#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include <string>
#include "core_variant/type_id.hpp"

class IClassDefinitionModifier;
class IDefinitionManager;

/**
 *	GenericDefinition
 */
class GenericDefinition
	: public IClassDefinitionDetails
{
private:
	friend class DefinitionManager;
	GenericDefinition( const char* name );
public:
	void init( IClassDefinitionModifier & ) override;

	ObjectHandle create( const IClassDefinition & definition ) const override;
	bool isAbstract() const override { return false; }
	bool isGeneric() const { return true; }
	const MetaBase * getMetaData() const { return nullptr; }
	const char * getParentName() const override { return nullptr; }
	const char * getName() const;

	ObjectHandle createBaseProvider(
		const ReflectedPolyStruct & polyStruct ) const override;

	ObjectHandle createBaseProvider(
		const IClassDefinition & definition, 
		const void * pThis ) const override;

	CastHelperCache * getCastHelperCache() const override { return &castHelperCache_; }

	IClassDefinitionModifier * getDefinitionModifier() const { return modifier_; }
private:
	const std::string name_;
	IClassDefinitionModifier * modifier_;
	mutable CastHelperCache castHelperCache_;
};

#endif //GENERIC_DEFINITION_HPP