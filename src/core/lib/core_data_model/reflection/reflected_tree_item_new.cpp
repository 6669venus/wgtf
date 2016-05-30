#include "reflected_tree_item_new.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"


namespace wgt
{
ReflectedTreeItemNew::ReflectedTreeItemNew( IComponentContext & contextManager,
	const ReflectedTreeModelNew & model )
	: parent_( nullptr )
	, path_( "" )
	, index_( -1 )
	, controller_( contextManager )
	, definitionManager_( contextManager )
	, model_( &model )
{
}


ReflectedTreeItemNew::ReflectedTreeItemNew( IComponentContext & contextManager,
	ReflectedTreeItemNew * parent,
	size_t index,
	const char * path ) 
	: parent_( parent )
	, index_( index )
	, path_( path )
	, controller_( contextManager )
	, definitionManager_( contextManager )
	, model_( parent != nullptr ? parent->getModel() : nullptr )
{
}


ReflectedTreeItemNew::ReflectedTreeItemNew( IComponentContext & contextManager,
	ReflectedTreeItemNew * parent,
	size_t index,
	const std::string & path ) 
	: parent_( parent )
	, index_( index )
	, path_( path )
	, controller_( contextManager )
	, definitionManager_( contextManager )
	, model_( parent != nullptr ? parent->getModel() : nullptr )
{
}


ReflectedTreeItemNew::~ReflectedTreeItemNew()
{
}


const IClassDefinition * ReflectedTreeItemNew::getDefinition() const 
{
	auto parent = this->getParent();
	if (parent == nullptr)
	{
		return nullptr;
	}
	return static_cast< const ReflectedTreeItemNew * >( parent )->getDefinition();
}


bool ReflectedTreeItemNew::isInPlace() const
{
	return false;
}


const std::string & ReflectedTreeItemNew::getPath() const
{
	return path_;
}


IReflectionController * ReflectedTreeItemNew::getController() const 
{
	if (controller_ != nullptr)
	{
		return controller_.get();
	}
	if (parent_ != nullptr)
	{
		return parent_->getController();
	}
	return nullptr;
}


IDefinitionManager * ReflectedTreeItemNew::getDefinitionManager() const 
{
	if (definitionManager_ != nullptr)
	{
		return definitionManager_.get();
	}
	if (parent_ != nullptr)
	{
		return parent_->getDefinitionManager();
	}
	return nullptr;
}


const ReflectedTreeModelNew * ReflectedTreeItemNew::getModel() const
{
	assert( (model_ != nullptr) && "Tree item is not attached to model" );
	return model_;
}


const ReflectedTreeItemNew * ReflectedTreeItemNew::getParent() const
{
	return parent_;
}


ReflectedTreeItemNew * ReflectedTreeItemNew::getParent()
{
	return parent_;
}


size_t ReflectedTreeItemNew::getIndex() const
{
	return index_;
}


void ReflectedTreeItemNew::setIndex( size_t index )
{
	index_ = index;
}


bool ReflectedTreeItemNew::enumerateVisibleProperties(
	const PropertyCallback & callback ) const
{
	auto object = this->getObject();
	if (object == nullptr)
	{
		return true;
	}

	auto pDefinitionManager = this->getDefinitionManager();
	if (pDefinitionManager == nullptr)
	{
		return true;
	}

	return enumerateVisibleProperties( object, *pDefinitionManager, "", callback );
}


bool ReflectedTreeItemNew::enumerateVisibleProperties( ObjectHandle object,
	const IDefinitionManager & definitionManager,
	const std::string & inPlacePath,
	const PropertyCallback & callback )
{
	auto definition = object.getDefinition( definitionManager );
	if (definition == nullptr)
	{
		return true;
	}

	for (const auto & property : definition->allProperties())
	{
		assert( property != nullptr );
		auto inPlace = findFirstMetaData< MetaInPlaceObj >( *property, definitionManager );
		if (inPlace != nullptr)
		{
			auto propertyAccessor = definition->bindProperty( property->getName(), object );

			if (!propertyAccessor.canGetValue())
			{
				continue;
			}

			const Variant & value = propertyAccessor.getValue();
			const bool isCollection = value.typeIs< Collection >();
			if (isCollection)
			{
				// TODO: Support InPlace collections
				// For now just show the collection
				// Currently Despair's ContainerProperty is pushed up to the collection
				// Eventually we'll need a MetaContainer which we can check here

				if (!callback( property, inPlacePath ))
				{
					return false;
				}
			}

			ObjectHandle handle;
			const bool isObjectHandle = value.tryCast(handle);
			if (isObjectHandle)
			{
				handle = reflectedRoot( handle, definitionManager );
				auto path = inPlacePath + property->getName() + ".";
				if (!enumerateVisibleProperties( handle,
					definitionManager,
					path,
					callback ))
				{
					return false;
				}
			}
			continue;
		}
		const bool isHidden =
			findFirstMetaData< MetaHiddenObj >( *property, definitionManager ) != nullptr;
		if (isHidden)
		{
			continue;
		}
		if (!callback( property, inPlacePath ))
		{
			return false;
		}
	}
	return true;
}
} // end namespace wgt
