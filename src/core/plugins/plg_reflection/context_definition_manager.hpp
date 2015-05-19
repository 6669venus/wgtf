#ifndef CONTEXT_DEFINITION_MANAGER_HPP
#define CONTEXT_DEFINITION_MANAGER_HPP

#include <set>

#include "reflection/i_definition_manager.hpp"
#include "dependency_system/i_interface.hpp"

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
	IClassDefinition * getDefinition( const char * name ) const;

	IClassDefinition * registerDefinition(
		IClassDefinitionDetails * definition,
		IClassDefinitionModifier ** o_Modifier = nullptr );

	bool deregisterDefinition( IClassDefinition * definition );

	bool serializeDefinitions( IDataStream & dataStream ) override;
	bool deserializeDefinitions( IDataStream & dataStream ) override;

	void getDefinitionsOfType( const IClassDefinition * definition,
		std::vector< IClassDefinition * > & o_Definitions ) const;

	void getDefinitionsOfType( const std::string & type,
		std::vector< IClassDefinition * > & o_Definitions ) const;

	IObjectManager * getObjectManager() const;

	void registerPropertyAccessorListener(
		std::shared_ptr< PropertyAccessorListener > & listener );

	void deregisterPropertyAccessorListener(
		std::shared_ptr< PropertyAccessorListener > & listener );

	const PropertyAccessorListeners & getPropertyAccessorListeners() const;

	GenericProperty * createGenericProperty( const char * name, const char * typeName );

	IClassDefinitionDetails * createGenericDefinition( const char * name ) const override;
private:
	IDefinitionManager * pBaseManager_;
	std::set<IClassDefinition *> contextDefinitions_;
	const std::wstring contextName_;
};

#endif // CONTEXT_DEFINITION_MANAGER_HPP
