#ifndef OBJECT_MANAGER_HPP
#define OBJECT_MANAGER_HPP

#include <unordered_set>
#include <unordered_map>
#include <map>

#include "i_object_manager.hpp"
#include "reflected_object.hpp"
#include "ref_object_id.hpp"

struct ObjectMetaData;

typedef std::unordered_set< ObjectMetaData * > ObjIdSet;

/**
 * ObjectManager
 */
class ObjectManager 
	: public Implements< IObjectManager >
{
public:
	// IObjectManager
	ObjectHandle getObject( const RefObjectId& id ) const override;
	ObjectHandle getObject( const void * pObj ) const override;
	ObjectHandle getUnmanagedObject( const void * pObj ) const override;

	bool getContextObjects( IDefinitionManager * context,
		std::vector< RefObjectId > & o_objects ) const override;
	void getObjects( std::vector< ObjectHandle > & o_objects ) const override;

	ObjectHandle registerObject(
		ObjectHandle & object, 
		const RefObjectId& id = RefObjectId::zero() ) override;

	RefObjectId registerUnmanagedObject(
		const ObjectHandle & object, 
		const RefObjectId& id = RefObjectId::zero() ) override;

	bool registerContext( IDefinitionManager * context ) override;
	bool deregisterContext( IDefinitionManager * context ) override;

	void registerListener( IObjectManagerListener * listener ) override;
	void deregisterListener( IObjectManagerListener * listener ) override;

	ISerializationManager * getSerializationManager() override;
	const ISerializationManager * getSerializationManager() const override;

	bool saveObjects( IDataStream& dataStream, IDefinitionManager & defManager ) override;
	bool loadObjects( IDataStream& dataStream, IDefinitionManager & defManager ) override;
	void addObjectLinks( const std::string & objId, PropertyAccessor & pa ) override;

	ObjectManager();
	virtual ~ObjectManager();
	void init( IDefinitionManager * pDefManager );
	void setSerializationManager(ISerializationManager * pSerilizationMgr);

private:
	ObjectHandle createObject( 
		const RefObjectId & id,
		const std::string & classDef ) const override;

	ObjectHandle create( 
		const std::string & classDef ) const override;

	void resolveObjectLink( const RefObjectId & objId, const ObjectHandle& object );

	void deregisterMetaData( ObjectMetaData & metaData );

	void NotifyObjectRegistred( const ObjectHandle & handle ) const;
	void NotifyObjectDeregistred( const ObjectHandle & handle ) const;

private:
	// all references to a reflected object
	// maps id to reflected object & its references
	std::unordered_map< const RefObjectId, std::weak_ptr< ObjectMetaData > > idMap_;
	std::unordered_map< const void *, std::weak_ptr< ObjectMetaData > > metaDataMap_;
	std::unordered_map< const void *, std::shared_ptr< ObjectMetaData > > unmanagedMetaDataMap_;
	mutable std::mutex objectsLock_;

	// all objects of a definition context
	// maps definition context to all its objects
	typedef std::unordered_map< IDefinitionManager *, std::shared_ptr< ObjIdSet > > ContextObjects;
	ContextObjects contextObjects_;

	IDefinitionManager * pDefManager_;

	ISerializationManager * pSerializationManager_;

	typedef std::vector< IObjectManagerListener * > ObjectManagerListener;
	ObjectManagerListener listeners_;

	mutable std::mutex listenersLock_;

	std::unordered_map< const RefObjectId, PropertyAccessor > objLink_;
	mutable std::mutex objLinkLock_;
};

#endif // OBJECT_MANAGER_HPP