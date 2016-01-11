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


template< class T >
class Connection
{
public:
	Connection( std::shared_ptr< T > & pObj )
		: ptr_( pObj.get() )
		, holder_( pObj )
	{
	}

	bool operator ==( const Connection< T > & other )
	{
		return other.ptr_ == ptr_;
	}

	std::shared_ptr< T > get() const
	{
		assert( holder_.lock().get() != nullptr );
		return holder_.lock();
	}
private:
	T *					ptr_;
	std::weak_ptr< T >	holder_;
};


/**
 * IDefinitionManager
 */
class IDefinitionManager
{
public:
	virtual ~IDefinitionManager() {}

	typedef MutableVector< Connection< PropertyAccessorListener > > PropertyAccessorListeners;

	virtual IClassDefinition * getDefinition( const char * name ) const = 0;
	virtual IClassDefinition * getObjectDefinition( const ObjectHandle & object ) const = 0;

	virtual IClassDefinitionDetails * createGenericDefinition( const char * name ) const = 0;

	virtual void getDefinitionsOfType( const IClassDefinition * definition,
		std::vector< IClassDefinition * > & o_Definitions ) const = 0;

	virtual void getDefinitionsOfType( const std::string & type,
		std::vector< IClassDefinition * > & o_Definitions ) const = 0;

	virtual IObjectManager * getObjectManager() const = 0;

	virtual IClassDefinition * registerDefinition(
		IClassDefinitionDetails * definition, 
		IClassDefinitionModifier ** o_Modifier = nullptr ) = 0;
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
