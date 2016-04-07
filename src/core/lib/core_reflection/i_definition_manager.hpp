#ifndef I_DEFINITION_MANAGER_HPP
#define I_DEFINITION_MANAGER_HPP

#include <string>
#include <vector>

#include "mutable_vector.hpp"
#include "core_serialization/i_datastream.hpp"
#include "reflected_object.hpp"
#include "object_handle.hpp"

class IClassDefinitionModifier;
class IClassDefinitionDetails;
class IClassDefinition;
class IDefinitionHelper;
class IObjectManager;
class PropertyAccessorListener;
class ISerializer;

/**
 * IDefinitionManager
 */
class IDefinitionManager
{
public:
	virtual ~IDefinitionManager() {}

	typedef MutableVector< std::weak_ptr< PropertyAccessorListener > > PropertyAccessorListeners;

	/**
	 *	Get a definition for the type represented by 'name'.
	 */
	virtual IClassDefinition * getDefinition( const char * name ) const = 0;
	/**
	 *	Get a definition for an object instance. Will fall back to object type if no definition can be found for the specific instance
	 */
	virtual IClassDefinition * getObjectDefinition( const ObjectHandle & object ) const = 0;

	virtual IClassDefinitionDetails * createGenericDefinition( const char * name ) const = 0;

	virtual void getDefinitionsOfType( const IClassDefinition * definition,
		std::vector< IClassDefinition * > & o_Definitions ) const = 0;

	virtual void getDefinitionsOfType( const std::string & type,
		std::vector< IClassDefinition * > & o_Definitions ) const = 0;

	virtual IObjectManager * getObjectManager() const = 0;

	virtual IClassDefinition * registerDefinition( IClassDefinitionDetails * definition ) = 0;
	virtual bool deregisterDefinition( const IClassDefinition * definition ) = 0;

	virtual void registerDefinitionHelper( const IDefinitionHelper & helper ) = 0;
	virtual void deregisterDefinitionHelper( const IDefinitionHelper & helper ) = 0;

	virtual void registerPropertyAccessorListener(
		std::shared_ptr< PropertyAccessorListener > & listener ) = 0;
	virtual void deregisterPropertyAccessorListener(
		std::shared_ptr< PropertyAccessorListener > & listener ) = 0;
	virtual const PropertyAccessorListeners & getPropertyAccessorListeners() const = 0;

	virtual bool serializeDefinitions( ISerializer & serializer ) = 0;
	virtual bool deserializeDefinitions( ISerializer & serializer ) = 0;

	template< typename TargetType >
	IClassDefinition * getDefinition() const
	{
		const char * defName = ::getClassIdentifier< TargetType >();
		return getDefinition( defName );
	}


	template< class T >
	ObjectHandleT< T > create( bool managed = true ) const
	{
		auto definition = getDefinition< T >();
		if (definition == nullptr)
		{
			return ObjectHandleT< T >();
		}
		auto object = managed ? definition->createManagedObject() : definition->create();
		return safeCast< T >( object );
	}
};

#endif // I_DEFINITION_MANAGER_HPP
