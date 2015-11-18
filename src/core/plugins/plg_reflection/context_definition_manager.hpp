#ifndef CONTEXT_DEFINITION_MANAGER_HPP
#define CONTEXT_DEFINITION_MANAGER_HPP

#include <set>

#include "core_reflection/i_definition_manager.hpp"
#include "core_dependency_system/i_interface.hpp"

class GenericProperty;

class ContextDefinitionManager
	: public Implements< IDefinitionManager >
{
public:
	ContextDefinitionManager( const wchar_t * contextName );
	virtual ~ContextDefinitionManager();

	void init( IDefinitionManager * pBaseManager );

	
	IDefinitionManager * getBaseManager() const;

private:
	// IDefinitionManager
	IClassDefinition * getDefinition( const char * name ) const override;

	IClassDefinition * registerDefinition(
		IClassDefinitionDetails * definition,
		IClassDefinitionModifier ** o_Modifier = nullptr ) override;

	bool deregisterDefinition( IClassDefinition * definition ) override;

	bool serializeDefinitions( ISerializer & serializer ) override;
	bool deserializeDefinitions( ISerializer & serializer ) override;

	void getDefinitionsOfType( const IClassDefinition * definition,
		std::vector< IClassDefinition * > & o_Definitions ) const override;

	void getDefinitionsOfType( const std::string & type,
		std::vector< IClassDefinition * > & o_Definitions ) const override;

	IObjectManager * getObjectManager() const override;

	void registerPropertyAccessorListener(
		std::shared_ptr< PropertyAccessorListener > & listener ) override;

	void deregisterPropertyAccessorListener(
		std::shared_ptr< PropertyAccessorListener > & listener ) override;

	const PropertyAccessorListeners & getPropertyAccessorListeners() const override;

	GenericProperty * createGenericProperty( const char * name, const char * typeName );

	IClassDefinitionDetails * createGenericDefinition( const char * name ) const override;
private:
	IDefinitionManager * pBaseManager_;
	std::set<IClassDefinition *> contextDefinitions_;
	const std::wstring contextName_;
};

#endif // CONTEXT_DEFINITION_MANAGER_HPP
