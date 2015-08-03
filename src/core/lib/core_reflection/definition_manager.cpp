#include "definition_manager.hpp"

#include "class_definition.hpp"
#include "property_accessor_listener.hpp"
#include "i_object_manager.hpp"
#include "metadata/meta_base.hpp"
#include "interfaces/i_base_property.hpp"
#include "generic/generic_definition.hpp"
 
namespace
{
	void initMetaData(
		const MetaBase * metaBase, 
		IDefinitionManager * definitionManager )
	{
		while( metaBase != nullptr )
		{
			auto metaBaseDefinition =
				definitionManager->getDefinition( metaBase->getDefinitionName() );
			assert( metaBaseDefinition != nullptr );
			const_cast< MetaBase * >( metaBase )->setDefinition( metaBaseDefinition );
			metaBase = metaBase->next();
		}
	}
}

//==============================================================================
DefinitionManager::DefinitionManager( IObjectManager & objectManager )
	: objectManager_ ( objectManager )
{
}


//==============================================================================
DefinitionManager::~DefinitionManager()
{
	listeners_.clear();

	for( ClassDefCollection::const_iterator it = definitions_.begin();
		it != definitions_.end();
		++it )
	{
		delete it->second;
	}
}


//==============================================================================
IClassDefinition * DefinitionManager::getDefinition( const char * name ) const
{
	assert( name );
	ClassDefCollection::const_iterator findIt = definitions_.find( name );
	if (findIt != definitions_.end())
	{
		return findIt->second;
	}
	return nullptr;
}


//------------------------------------------------------------------------------
IClassDefinitionDetails * DefinitionManager::createGenericDefinition(
	const char * name ) const
{
	return new GenericDefinition( name );
}


//==============================================================================
IClassDefinition * DefinitionManager::registerDefinition(
	IClassDefinitionDetails * defDetails,
	IClassDefinitionModifier ** o_Modifier )
{
	assert( defDetails );
	IClassDefinitionModifier * modifier = nullptr;
	auto definition = new ClassDefinition( defDetails, &modifier );
	std::pair< ClassDefCollection::iterator, bool > insertIt =
		definitions_.insert(
			std::make_pair( definition->getName(), definition ) );
	modifier->setDefinitionManager( this );
	const MetaBase * metaBase = definition->getMetaData();
	initMetaData( metaBase, this );

	for(PropertyIterator pi = definition->directProperties().begin();
		pi.current(); pi.next() )
	{
		metaBase = pi.current()->getMetaData();
		initMetaData( metaBase, this );
	}

	if(o_Modifier)
	{
		*o_Modifier = modifier;
	}

	return definition;
}

//==============================================================================
bool DefinitionManager::deregisterDefinition( IClassDefinition * definition )
{
	assert( definition );
	ClassDefCollection::iterator it = 
		definitions_.find( definition->getName() );
	assert( it != definitions_.end() );
	if (it == definitions_.end())
	{
		return false;
	}
	definitions_.erase( it );
	return true;
}


//==============================================================================
void DefinitionManager::getDefinitionsOfType(
	const IClassDefinition * definition,
	std::vector< IClassDefinition * > & o_Definitions ) const
{
	getDefinitionsOfType( definition->getName(),o_Definitions );
}


//==============================================================================
void DefinitionManager::getDefinitionsOfType( const std::string & baseType,
											 std::vector< IClassDefinition * > & o_Definitions ) const
{
	ClassDefCollection::const_iterator findIt = definitions_.find( baseType.c_str() );
	if (findIt == definitions_.end())
	{
		return;
	}
	IClassDefinition * baseDefinition = findIt->second;
	o_Definitions.push_back( baseDefinition );
	getDefinitionsOfType( baseDefinition, o_Definitions, o_Definitions.size() );
}


//==============================================================================
void DefinitionManager::getDefinitionsOfType(
	IClassDefinition * definition,
	std::vector< IClassDefinition * > & o_Definitions,
	size_t startIndex ) const
{
	for( ClassDefCollection::const_iterator it = definitions_.begin(); 
		it != definitions_.end(); ++it )
	{
		if( it->second->getParent() == definition )
		{
			o_Definitions.push_back( it->second );
		}
	}

	size_t endIndex = o_Definitions.size();
	for( size_t i = startIndex; i < endIndex; ++i )
	{
		getDefinitionsOfType(
			o_Definitions[ i ], o_Definitions, o_Definitions.size() );
	}
}


//==============================================================================
void DefinitionManager::registerPropertyAccessorListener(
	std::shared_ptr< PropertyAccessorListener > & listener )
{
	listeners_.push_back( Connection< PropertyAccessorListener >( listener ) );
}


//==============================================================================
void DefinitionManager::deregisterPropertyAccessorListener(
	std::shared_ptr< PropertyAccessorListener > & listener )
{
	listeners_.erase( Connection< PropertyAccessorListener>( listener ) );
}

//==============================================================================
const IDefinitionManager::PropertyAccessorListeners &
	DefinitionManager::getPropertyAccessorListeners() const
{
	return listeners_;
}

//==============================================================================
IObjectManager * DefinitionManager::getObjectManager() const
{
	return &objectManager_;
}

bool DefinitionManager::serializeDefinitions( IDataStream & dataStream )
{
	return false;
}
bool DefinitionManager::deserializeDefinitions( IDataStream & dataStream )
{
	return false;
}
