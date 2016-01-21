#ifndef GENERIC_DEFINITION_HPP
#define GENERIC_DEFINITION_HPP

#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "core_reflection/metadata/meta_base.hpp"
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
	bool isGeneric() const override { return true; }
	MetaHandle getMetaData() const override { return nullptr; }
	const char * getParentName() const override { return nullptr; }
	const char * getName() const override;

	CastHelperCache * getCastHelperCache() const override { return &castHelperCache_; }
	void * upCast( void * object ) const override { return nullptr; }

	IClassDefinitionModifier * getDefinitionModifier() const { return modifier_; }
private:
	const std::string name_;
	IClassDefinitionModifier * modifier_;
	mutable CastHelperCache castHelperCache_;
};

#endif //GENERIC_DEFINITION_HPP