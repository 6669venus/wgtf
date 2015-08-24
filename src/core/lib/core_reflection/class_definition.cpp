#include "class_definition.hpp"
#include "base_property.hpp"
#include "reflected_object.hpp"
#include "property_accessor.hpp"
#include "property_iterator.hpp"
#include "i_definition_manager.hpp"
#include "i_object_manager.hpp"

#include "interfaces/i_class_definition_details.hpp"

#include "metadata/meta_base.hpp"
#include "metadata/meta_utilities.hpp"
#include "metadata/meta_impl.hpp"

#include "utilities/definition_helpers.hpp"

#include "core_variant/variant.hpp"

namespace
{ 
	//==========================================================================
	class BasePropertyWithMetaData
		: public IBaseProperty
	{
	public:
		//----------------------------------------------------------------------
		BasePropertyWithMetaData(
			IBaseProperty * pBase, const MetaBase * metaBase )
			: pBase_( pBase )
			, pMetaData_( metaBase )
		{
		}


		//----------------------------------------------------------------------
		const TypeId & getType() const override
		{
			return pBase_->getType();
		}


		//----------------------------------------------------------------------
		const char * getName() const override
		{
			return pBase_->getName();
		}


		//----------------------------------------------------------------------
		const MetaBase * getMetaData() const override
		{
			return pMetaData_.get();
		}


		//----------------------------------------------------------------------
		bool set(
			const ObjectHandle & handle, const Variant & value, const IDefinitionManager & definitionManager ) const override
		{
			return pBase_->set( handle, value, definitionManager );
		}


		//----------------------------------------------------------------------
		Variant get( const ObjectHandle & handle, const IDefinitionManager & definitionManager ) const override
		{
			return pBase_->get( handle, definitionManager );
		}

	private:
		std::unique_ptr< IBaseProperty >	pBase_;
		std::unique_ptr< const MetaBase >	pMetaData_;
	};


	//==========================================================================
	class CollectionElementHolder
		: public BaseProperty
	{
	public:
		//======================================================================
		// Move constructor
		//======================================================================
		CollectionElementHolder( CollectionElementHolder && other )
			: BaseProperty( other.getName(), other.getType() )
			, collectionIt_( std::move( other.collectionIt_ ) )
		{
		}

		CollectionElementHolder()
			: BaseProperty( "", TypeId::getType< void >() )
			, collectionIt_( NULL )
		{
		}

		//======================================================================
		void setType( const TypeId & type )
		{
			BaseProperty::setType( type );
		}


		//======================================================================
		const Collection::Iterator & getIterator() const
		{
			return collectionIt_;
		}

		//======================================================================
		void setIterator( Collection::Iterator & collectionIt )
		{
			collectionIt_ = std::move( collectionIt );
		}


		//======================================================================
		Variant get( const ObjectHandle & pBase, const IDefinitionManager & definitionManager ) const override
		{
			return collectionIt_.value();
		}

		//======================================================================
		bool set( const ObjectHandle &, const Variant & value, const IDefinitionManager & definitionManager ) const override
		{
			return collectionIt_.setValue( value );
		}


		//======================================================================
		void setName( const char * name )
		{
			propName_ = name;
			BaseProperty::setName( propName_.c_str() );
		}


	private:
		mutable Collection::Iterator		collectionIt_;
		std::string							propName_;
	};

	static const char s_CollectionKeyBegin = '[';
	static const char s_CollectionKeyEnd = ']';

	bool handleCollection(
		CollectionElementHolder & ceh,
		Collection & collection,
		char * nameBuffer,
		const char * propNameBegin,
		const char * & o_PropNameEnd )
	{
		if (*propNameBegin != s_CollectionKeyBegin)
		{
			return false;
		}
		//TODO : support non-integer keys
		auto index = atol( propNameBegin + 1 );

		auto findIt = collection.find( index );
		if (findIt == collection.end())
		{
			return true;
		}
		const TypeId valueType = collection.valueType();
		ceh.setType( valueType );
		//collection is possibly no longer valid after following function call
		ceh.setIterator( findIt );
		o_PropNameEnd = strchr( propNameBegin + 1, s_CollectionKeyEnd );
		if (*o_PropNameEnd == '\0')
		{
			return true;
		}
		nameBuffer += sprintf( nameBuffer, "[%lu]", index );
		if (*(o_PropNameEnd + 1 ) =='\0')
		{
			return true;
		}
		auto & it = ceh.getIterator();
		Collection subCollection;
		bool isSubCollection = it.value().tryCast( subCollection );
		if(isSubCollection)
		{
			return handleCollection(
				ceh,
				subCollection,
				nameBuffer,
				o_PropNameEnd + 1,
				o_PropNameEnd );
		}
		return false;
	}
}

//------------------------------------------------------------------------------
ClassDefinition::ClassDefinition(
	IClassDefinitionDetails * details, IClassDefinitionModifier ** o_Modifier )
	: details_( details )
{
	details->init( *this );
	if (o_Modifier)
	{
		*o_Modifier = this;
	}
}


//------------------------------------------------------------------------------
const IClassDefinitionDetails & ClassDefinition::getDetails() const
{
	return *details_;
}


//------------------------------------------------------------------------------
PropertyIteratorRange ClassDefinition::allProperties() const
{
	return PropertyIteratorRange(this, PropertyIterator::ITERATE_PARENTS);
}


//------------------------------------------------------------------------------
PropertyIteratorRange ClassDefinition::directProperties() const
{
	return PropertyIteratorRange(this, PropertyIterator::ITERATE_SELF_ONLY);
}


//------------------------------------------------------------------------------
const SortedPropertyCollection & ClassDefinition::sortedProperties() const
{
	return sortedProperties_;
}


//------------------------------------------------------------------------------
PropertyIterator ClassDefinition::getPropertyIterator(
	PropertyIterator::IterateStrategy strategy ) const
{
	return PropertyIterator( this, strategy );
}


//------------------------------------------------------------------------------
bool ClassDefinition::operator == ( const ClassDefinition & other ) const
{
	return sortedProperties() == other.sortedProperties() &&
		getParent() == other.getParent() &&
		getMetaData() == other.getMetaData();
}


//------------------------------------------------------------------------------
bool ClassDefinition::operator != ( const ClassDefinition & other ) const
{
	return !operator ==( other );
}


//------------------------------------------------------------------------------
void ClassDefinition::addProperty(
	IBaseProperty * reflectedProperty,
	const MetaBase * metaBase )
{
	auto storedProperty = metaBase
		? new BasePropertyWithMetaData( reflectedProperty, metaBase )
		: reflectedProperty;

	properties_.insert(
		std::make_pair(
		reflectedProperty->getName(),
		std::unique_ptr< IBaseProperty >( storedProperty ) ) );
	sortedProperties_.push_back( storedProperty );
}


//------------------------------------------------------------------------------
IClassDefinition * ClassDefinition::getParent() const
{
	if (details_->getParentName() == NULL)
	{
		return NULL;
	}
	return defManager_->getDefinition( details_->getParentName() );
}


//------------------------------------------------------------------------------
PropertyAccessor ClassDefinition::bindProperty(
	const char * name, const ObjectHandle & object ) const
{
	assert( this == object.getDefinition() );
	PropertyAccessor propAccessor( getDefinitionManager(), object, name );
	bindPropertyImpl( name, object, propAccessor );
	return std::move( propAccessor );
}


/*------------------------------------------------------------------------------
 *	This method creates a PropertyAccessor bound to a property on a
 *	ReflectedPolyStruct.
 *	@param baseProperty base/owning ReflectedObject of the
 *		ReflectedPolyStruct.
 *	@param name the name of the property to bind.
 *	@param refPolyStruct the ReflectedPolyStruct on which to access the
 *		property.
 *		Note: This ClassDefinition must match the ClassDefinition on the
 *		refPolyStruct.
 */
PropertyAccessor ClassDefinition::bindPropertyPolyStruct(
	const PropertyAccessor & baseProperty,
	const char * name, ReflectedPolyStruct & refPolyStruct ) const
{
	assert( this == &(refPolyStruct.getDefinition()) );
	PropertyAccessor propAccessor( 
		getDefinitionManager(),
		baseProperty.getRootObject(), name );
	auto provider = getBaseProvider( &refPolyStruct );
	bindPropertyImpl( name, provider, propAccessor );
	return propAccessor;

}


//------------------------------------------------------------------------------
/**
 *	This method creates a PropertyAccessor bound to a property on a
 *	ReflectedStruct.
 *	@param baseProperty base/owning ReflectedObject of the
 *		ReflectedStruct.
 *	@param name the name of the property to bind.
 *	@param refStruct the ReflectedStruct on which to access the
 *		property.
 *		Note: This ClassDefinition must match the struct definition on the
 *		owning baseProperty object.
 */
PropertyAccessor ClassDefinition::bindPropertyAnon(
	const PropertyAccessor & baseProperty,
	const char * name, ObjectHandle & baseProvider ) const
{
	PropertyAccessor propAccessor(
		getDefinitionManager(),
		baseProperty.getRootObject(), name );
	bindPropertyImpl( name, baseProvider, propAccessor );
	return propAccessor;
}


//------------------------------------------------------------------------------
void ClassDefinition::bindPropertyImpl(
	const char * name, const ObjectHandle & pBase,
	PropertyAccessor & o_PropertyAccessor ) const
{
	if (strlen( name ) == 0)
	{
		return;
	}

	size_t charsToCmp = strlen( name );
	std::string strRef( name, charsToCmp );
	size_t foundPos = strRef.find_first_of( "[." );
	if (foundPos != strRef.npos)
	{
		charsToCmp = foundPos;
		strRef = std::string( name, charsToCmp );
	}
	
	o_PropertyAccessor.setObject( pBase );

	auto foundProp = findProperty( strRef.c_str() );
	if (foundProp == nullptr)
	{
		return;
	}
	o_PropertyAccessor.setBaseProperty( foundProp );

	if (strRef != o_PropertyAccessor.getName())
	{
		o_PropertyAccessor.setBaseProperty( NULL );
		return;
	}

	const char * newBegin = name + charsToCmp;

	if (*newBegin == '\0')
	{
		return;
	}

	Collection collection;
	bool isCollection = o_PropertyAccessor.getValue().tryCast( collection );

	ObjectHandle baseProvider;
	if (isCollection)
	{
		CollectionElementHolder ceh;
		const char * indexEnd = newBegin;
		char nameBuffer[ 256 ];
		if(handleCollection(
			ceh, collection,
			nameBuffer,
			newBegin, indexEnd))
				{
			std::shared_ptr< CollectionElementHolder > overrideBaseProperty(
				new CollectionElementHolder( std::move( ceh ) ) );
			overrideBaseProperty->setName( nameBuffer );
			o_PropertyAccessor.setObject( pBase );
			o_PropertyAccessor.setBaseProperty(
				overrideBaseProperty );
			return;
		}
		auto & findIt = ceh.getIterator();

		bool ok = findIt.value().tryCast( baseProvider );
		assert( ok );

		auto definition = baseProvider.getDefinition();
		if (definition == nullptr)
		{
			return;
		}
		return definition->bindPropertyImpl(
			indexEnd + + strlen( "]" ) + strlen( "." ),
			baseProvider,
			o_PropertyAccessor );
	}

	const char * childName = newBegin + strlen( "." ); // Skip .
	o_PropertyAccessor.getValue().tryCast( baseProvider );

	auto definition = baseProvider.getDefinition();
	if (definition == nullptr)
	{
		return;
	}

	return definition->bindPropertyImpl(
		childName,
		baseProvider,
		o_PropertyAccessor );
}


//==============================================================================
IBaseProperty * ClassDefinition::findProperty( const TypeId & propertyId ) const
{
	auto findIt = properties_.find( propertyId );
	if (findIt != properties_.end())
	{
		return findIt->second.get();
	}
	auto parentDef = getParent();
	if (parentDef == nullptr)
	{
		return nullptr;
	}
	return parentDef->findProperty( propertyId );
}


//------------------------------------------------------------------------------
bool ClassDefinition::isGeneric() const
{
	return getDetails().isGeneric();
}


//------------------------------------------------------------------------------
bool ClassDefinition::canBeCastTo( const IClassDefinition & definition ) const
{
	const IClassDefinition * baseDefinition = this;
	const char * definitionName = definition.getName();
	while( baseDefinition != NULL )
	{
		const char * baseName = baseDefinition->getName();
		if (strcmp( baseName, definitionName ) == 0)
		{
			return true;
		}
		baseDefinition = baseDefinition->getParent();
	}
	return false;
}


//------------------------------------------------------------------------------
void * ClassDefinition::castTo( const IClassDefinition & definition, void * object ) const
{
	const char * definitionName = definition.getName();

	const IClassDefinition * baseDefinition = this;
	while( baseDefinition != NULL )
	{
		const char * baseName = baseDefinition->getName();
		if (strcmp( baseName, definitionName ) == 0)
		{
			return object;
		}
		object = baseDefinition->upCast( object );
		baseDefinition = baseDefinition->getParent();
	}

	return nullptr;
}


//------------------------------------------------------------------------------
void * ClassDefinition::upCast( void * object ) const
{
	return details_->upCast( object );
}


//------------------------------------------------------------------------------
ObjectHandle ClassDefinition::create() const
{
	return details_->create( *this );
}


//------------------------------------------------------------------------------
ObjectHandle ClassDefinition::createManagedObject( const RefObjectId & id ) const
{
	RefObjectId newId = id;
	auto metaId = findFirstMetaData< MetaUniqueIdObj >( *this );
	if( metaId != nullptr)
	{
		std::string strId = metaId->getId();
		newId = strId;
	}
	ObjectHandle pObject = create();
	return registerObject( pObject, newId );
}


//------------------------------------------------------------------------------
const char * ClassDefinition::getName() const
{
	return details_->getName();
}


//------------------------------------------------------------------------------
const MetaBase * ClassDefinition::getMetaData() const
{
	return details_->getMetaData();
}


//------------------------------------------------------------------------------
void ClassDefinition::setDefinitionManager( IDefinitionManager * defManager )
{
	defManager_ = defManager;
}


//------------------------------------------------------------------------------
IDefinitionManager * ClassDefinition::getDefinitionManager() const
{
	return defManager_;
}


//------------------------------------------------------------------------------
ObjectHandle ClassDefinition::getBaseProvider(
	const ReflectedPolyStruct * polyStruct ) const
{
	if (polyStruct == nullptr)
	{
		return ObjectHandle();
	}
	assert( &polyStruct->getDefinition() == this );
	return details_->createBaseProvider( *polyStruct );
}


//------------------------------------------------------------------------------
ObjectHandle ClassDefinition::getBaseProvider( const void * pThis ) const
{
	return details_->createBaseProvider( *this, pThis );
}


//------------------------------------------------------------------------------
ObjectHandle ClassDefinition::registerObject( ObjectHandle & pObj, 
												const RefObjectId & id ) const
{
	const IDefinitionManager * pDefManager = getDefinitionManager();
	if (pDefManager)
	{
		IObjectManager * pObjManager = pDefManager->getObjectManager();
		if (pObjManager)
		{
			return pObjManager->registerObject( pObj, id );
		}
	}
	return ObjectHandle();
}