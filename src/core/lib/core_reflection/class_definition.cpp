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
			setName( propName_.c_str() );
		}

		const TypeId & getType() const override
		{
			return collectionIt_.valueType();
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

			auto valueType = collectionIt_.valueType();
			if (valueType.isPointer())
			{
				auto targetType = valueType.removePointer();
				auto targetDefinition = definitionManager.getDefinition( targetType.getName() );
				ObjectHandle source;
				if (value.tryCast( source ))
				{
					auto target = reflectedCast( source, targetType, definitionManager );
					return collectionIt_.setValue( target );
				}
			}

			return collectionIt_.setValue( value );
		}

	private:
		Collection::Iterator collectionIt_;
		std::string propName_;

	};

}


const char INDEX_OPEN = '[';
const char INDEX_CLOSE = ']';
const char DOT_OPERATOR = '.';


//------------------------------------------------------------------------------
ClassDefinition::ClassDefinition( IClassDefinitionDetails * details )
	: details_( details )
{
}


//------------------------------------------------------------------------------
const IClassDefinitionDetails & ClassDefinition::getDetails() const
{
	return *details_;
}


//------------------------------------------------------------------------------
PropertyIteratorRange ClassDefinition::allProperties() const
{
	return PropertyIteratorRange( PropertyIterator::ITERATE_PARENTS, *this );
}


//------------------------------------------------------------------------------
PropertyIteratorRange ClassDefinition::directProperties() const
{
	return PropertyIteratorRange( PropertyIterator::ITERATE_SELF_ONLY, *this );
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


//------------------------------------------------------------------------------
void ClassDefinition::bindPropertyImpl(
	const char * name,
	const ObjectHandle & pBase,
	PropertyAccessor & o_PropertyAccessor ) const
{
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
			if (!*propOperator || (it == end))
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
IBasePropertyPtr ClassDefinition::findProperty( const char * name ) const
{
	// Some definitions allow you to lookup by name directly
	if (details_->canDirectLookupProperty())
	{
		return details_->directLookupProperty( name );
	}

	// Otherwise, perform a search
	auto nameHash = HashUtilities::compute( name );
	auto properties = allProperties();
	for (auto it = properties.begin(); it != properties.end(); ++it)
	{
		if (it->getNameHash() == nameHash)
		{
			return *it;
		}
	}
	return nullptr;
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