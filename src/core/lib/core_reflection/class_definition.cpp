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
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"

#include <algorithm>
#include <utility>

namespace
{ 
	//==========================================================================
	class BasePropertyWithMetaData
		: public IBaseProperty
	{
	public:
		//----------------------------------------------------------------------
		BasePropertyWithMetaData( 
			ClassDefinition * definition,
			IBaseProperty * pBase, 
			MetaHandle metaData )
			: definition_( definition )
			, pBase_( pBase )
			, metaData_( metaData )
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
		MetaHandle getMetaData() const override
		{
			return metaData_;
		}


		bool readOnly() const override
		{
			if (pBase_ != nullptr && pBase_->readOnly())
			{
				return true;
			}

			auto metaReadOnly = findFirstMetaData< MetaReadOnlyObj >( metaData_, *definition_->getDefinitionManager() );
			return metaReadOnly != nullptr;
		}


		virtual bool isMethod() const override
		{
			return pBase_->isMethod();
		}


		virtual bool isValue() const override
		{
			return pBase_->isValue();
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


		virtual Variant invoke( const ObjectHandle & object,
			const ReflectedMethodParameters & parameters ) override
		{
			return pBase_->invoke( object, parameters );
		}


		virtual size_t parameterCount() const override
		{
			return pBase_->parameterCount();
		}


	private:
		ClassDefinition * definition_;
		std::unique_ptr< IBaseProperty > pBase_;
		MetaHandle metaData_;
	};


	//==========================================================================
	class CollectionElementHolder:
		public BaseProperty
	{
		typedef BaseProperty base;

	public:
		CollectionElementHolder(
			const Collection::Iterator& collectionIt,
			const TypeId& valueType,
			std::string propName ):

			base( "", valueType ),
			collectionIt_( collectionIt ),
			propName_( std::move( propName ) )
		{
		}

		const TypeId & getType() const override
		{
			return collectionIt_.valueType();
		}

		const char * getName() const override
		{
			return propName_.c_str();
		}

		virtual bool isValue() const override
		{
			return true;
		}

		Variant get( const ObjectHandle & pBase, const IDefinitionManager & definitionManager ) const override
		{
			assert( this->isValue() );
			return collectionIt_.value();
		}

		bool set( const ObjectHandle &, const Variant & value, const IDefinitionManager & definitionManager ) const override
		{
			assert( !this->readOnly() );
			return collectionIt_.setValue( value );
		}

	private:
		Collection::Iterator collectionIt_;
		std::string propName_;

	};

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
	MetaHandle metaData )
{
	auto storedProperty = metaData != nullptr
		? new BasePropertyWithMetaData( this, reflectedProperty, metaData )
		: reflectedProperty;

	properties_.insert(
		std::make_pair(
		reflectedProperty->getName(),
		std::unique_ptr< IBaseProperty >( storedProperty ) ) );
	sortedProperties_.push_back( storedProperty );
}


//------------------------------------------------------------------------------
size_t ClassDefinition::getPropertyCount() const
{
	return properties_.size();
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
	assert( getDefinitionManager() );
	assert( this == object.getDefinition( *getDefinitionManager() ) );
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
	const char * name,
	const ObjectHandle & pBase,
	PropertyAccessor & o_PropertyAccessor ) const
{
	const char INDEX_OPEN = '[';
	const char INDEX_CLOSE = ']';
	const char DOT_OPERATOR = '.';

	if (!*name)
	{
		// empty name causes noop
		return;
	}

	// find property operator
	auto propOperator = name;
	while (true)
	{
		if( !*propOperator ||
			*propOperator == INDEX_OPEN ||
			*propOperator == DOT_OPERATOR )
		{
			break;
		}

		propOperator += 1;
	}

	auto propName = name;
	std::string propNameTmp;
	if (*propOperator)
	{
		// allocate temp string only to extract substring
		propNameTmp.assign( name, propOperator );
		propName = propNameTmp.c_str();
	}

	auto baseProp = findProperty( propName );
	if (baseProp == nullptr)
	{
		// error: property `propName` is not found
		o_PropertyAccessor.setBaseProperty( nullptr );
		return;
	}

	o_PropertyAccessor.setObject( pBase );
	o_PropertyAccessor.setBaseProperty( baseProp );

	assert( strcmp( propName, o_PropertyAccessor.getName() ) == 0 );

	if (!*propOperator)
	{
		// no operator, so that's it
		return;
	}

	Variant propVal = o_PropertyAccessor.getValue();
	if (*propOperator == INDEX_OPEN)
	{
		auto wholeIndex = propOperator;

		// read "multidimensional" indices without recursive bind (optimization)
		while (true)
		{
			Collection collection;
			if (!propVal.tryCast( collection ))
			{
				// error: index operator is applicable to collections only
				o_PropertyAccessor.setBaseProperty( nullptr );
				return;
			}

			// determine key type (heterogeneous keys are not supported yet)
			const auto begin = collection.begin();
			const auto end = collection.end();
			if (begin == end)
			{
				// error: can't index empty collection
				o_PropertyAccessor.setBaseProperty( nullptr );
				return;
			}

			// read key
			Variant key( begin.key().type() );
			{
				propOperator += 1; // skip INDEX_OPEN

				FixedMemoryStream dataStream( propOperator );
				TextStream stream( dataStream );

				stream >> key >> match( INDEX_CLOSE );

				if (stream.fail())
				{
					// error: either key can't be read, or it isn't followed by INDEX_CLOSE
					o_PropertyAccessor.setBaseProperty( nullptr );
					return;
				}

				// skip key and closing bracket
				propOperator += stream.seek( 0, std::ios_base::cur );
			}

			auto it = collection.find( key );
			// If (it == end), still return a valid property accessor to end,
			// rather than an invalid property accessor.

			if (!*propOperator)
			{
				// name parsing is completed
				auto baseProp = std::make_shared< CollectionElementHolder >(
					it,
					collection.valueType(),
					wholeIndex );
				// o_PropertyAccessor.setObject(); - keep current base
				o_PropertyAccessor.setBaseProperty( baseProp );
				return;
			}

			propVal = it.value();

			if (*propOperator == INDEX_OPEN)
			{
				continue;
			}

			// parse next operator
			break;
		}
	}

	if (*propOperator == DOT_OPERATOR)
	{
		ObjectHandle propObject;
		if (!propVal.tryCast( propObject ))
		{
			// error: dot operator is applicable to objects only
			o_PropertyAccessor.setBaseProperty( nullptr );
			return;
		}

		propObject = reflectedRoot( propObject, *getDefinitionManager() );
		auto definition = propObject.getDefinition( *getDefinitionManager() );
		if (definition == nullptr)
		{
			// error: dot operator applied to object of type which is not reflected
			o_PropertyAccessor.setBaseProperty( nullptr );
			return;
		}

		return definition->bindPropertyImpl(
			propOperator + 1, // skip dot
			propObject,
			o_PropertyAccessor );
	}

	// error: unknown operator at *propOperator
	o_PropertyAccessor.setBaseProperty( nullptr );
	return;
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
	auto metaId = findFirstMetaData< MetaUniqueIdObj >( *this, *defManager_ );
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
MetaHandle ClassDefinition::getMetaData() const
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