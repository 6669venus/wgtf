#ifndef I_OBJECT_MANAGER_HPP
#define I_OBJECT_MANAGER_HPP

#include "reflected_object.hpp"
#include "i_definition_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include <memory>

class ObjectHandle;
class RefObjectId;
class ISerializer;

class IObjectManagerListener
{
public:
	virtual ~IObjectManagerListener() {}
	virtual void onObjectRegistered( const ObjectHandle & pObj ) {}
	virtual void onObjectDeregistered( const ObjectHandle & pObj ) {}
};

/**
 * IObjectManager
 */
class IObjectManager
{
public:
	virtual ObjectHandle createObject( 
		const RefObjectId & id,
		const std::string & classDef ) const = 0;

	virtual ObjectHandle create( 
		const std::string & classDef ) const = 0;

	virtual ObjectHandle getObject( const RefObjectId & id ) const = 0;
	virtual ObjectHandle getObject( const void * pObj ) const = 0;
	virtual ObjectHandle getUnmanagedObject( const void * pObj ) const = 0;

	virtual bool getContextObjects( IDefinitionManager * context,
		std::vector< RefObjectId > & o_objects ) const = 0;
	virtual void getObjects( std::vector< ObjectHandle > & o_objects ) const = 0;

	virtual ObjectHandle registerObject(
		ObjectHandle & pObj, 
		const RefObjectId & id = RefObjectId::zero() ) = 0;

	virtual RefObjectId registerUnmanagedObject(
		const ObjectHandle & object, 
		const RefObjectId& id = RefObjectId::zero() ) = 0;
	
	virtual bool registerContext( IDefinitionManager * context ) = 0;
	virtual bool deregisterContext( IDefinitionManager * context ) = 0;

	virtual void registerListener( IObjectManagerListener * listener ) = 0;
	virtual void deregisterListener( IObjectManagerListener * listener ) = 0;

	virtual bool saveObjects( IDefinitionManager& contextDefinitionManager, ISerializer& serializer ) = 0;
	virtual bool loadObjects( ISerializer& serializer ) = 0;
	virtual void addObjectLinks( const std::string & objId, PropertyAccessor & pa ) = 0;
};

#endif // I_OBJECT_MANAGER_HPP
