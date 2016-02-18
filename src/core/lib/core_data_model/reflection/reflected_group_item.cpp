#include "reflected_group_item.hpp"
#include "reflected_property_item.hpp"

#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"

#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_data_model/i_item_role.hpp"

#include "core_string_utils/string_utils.hpp"
#include <codecvt>

ReflectedGroupItem::ReflectedGroupItem( const MetaGroupObj * groupObj, ReflectedItem * parent )
	: ReflectedItem( parent, parent->getPath() )
	, groupObj_( groupObj )
{
	assert( groupObj_ != nullptr );
	std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
	if (groupObj_ == nullptr)
	{
		displayName_.clear();
	}
	else
	{
		displayName_ = conversion.to_bytes( groupObj_->getGroupName() );
	}
	
}

const char * ReflectedGroupItem::getDisplayText( int column ) const
{
	switch (column)
	{
	case 0:
		return displayName_.c_str();

	case 1:
		return "Reflected Group";

	default:
		assert( false );
		return "";
	}
}

Variant ReflectedGroupItem::getData( int column, size_t roleId ) const
{
	auto obj = getObject();
	if (obj == nullptr)
	{
		return Variant();
	}
	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return Variant();
	}

	if (roleId == IndexPathRole::roleId_)
	{
		if (parent_ == nullptr)
		{
			return displayName_;
		}
		std::string parentIndexPath = parent_->getPath();
		return parentIndexPath + displayName_;
	}

	if (roleId == ValueRole::roleId_)
	{
		auto collectionHolder = std::make_shared< CollectionHolder< Variants > >();
		Variants& childValues_ = collectionHolder->storage();
				
		std::string childPath;
		getChildren(obj, childPath, childValues_);

		return std::move( Collection( collectionHolder ) );
	}
	return Variant();
}

void ReflectedGroupItem::getChildren(ObjectHandle obj, std::string &childPath, Variants &childValues) const
{
	if( groupObj_ == nullptr )
		return;

	auto definitionManager = getDefinitionManager();
	if( definitionManager == nullptr )
		return;

	auto definition = obj.getDefinition( *getDefinitionManager() );
	if( definition == nullptr )
		return;

	auto self = this;
	EnumerateVisibleProperties( obj, [&](IBasePropertyPtr property, const char*){
		// Check if this property is a part of this group
		const auto groupObj = findFirstMetaData< MetaGroupObj >(*property, *self->getDefinitionManager());
		if ( self->isSameGroup( groupObj ) )
		{
			childPath = self->path_ + property->getName();
			auto propertyAccessor = definition->bindProperty(childPath.c_str(), obj);
			const Variant & value = propertyAccessor.getValue();
			childValues.emplace_back(value);
		}
		return true;
	});
}

bool ReflectedGroupItem::isSameGroup(const MetaGroupObj* group) const
{
	return groupObj_ != nullptr && group != nullptr
		&& (group == groupObj_ || group->getGroupNameHash() == groupObj_->getGroupNameHash());
}

bool ReflectedGroupItem::setData( int column, size_t roleId, const Variant & data )
{
	auto controller = getController();
	if (controller == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	if (obj == nullptr)
	{
		return false;
	}
	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return false;
	}

	Collection collection;
	bool isOk = data.tryCast( collection );
	if(!isOk)
	{
		return false;
	}
	size_t value_size = collection.size();
	
	auto iter = collection.begin();

	EnumerateVisibleProperties(getObject(), [&](IBasePropertyPtr property, const char* groupPath)
	{
		if(iter == collection.end())
			return false;

		auto groupObj = findFirstMetaData< MetaGroupObj >( *property, *getDefinitionManager() );
		if ( isSameGroup( groupObj ) )
		{
			const Variant & value = *iter++;
			auto path = std::string(groupPath) + property->getName();
			auto propertyAccessor = definition->bindProperty( path.c_str(), obj );
			controller->setValue( propertyAccessor, value );
		}
		return true;
	});

	return true;
}

GenericTreeItem * ReflectedGroupItem::getChild( size_t index ) const
{
	GenericTreeItem * child = nullptr;
	if (children_.size() > index )
	{
		child = children_[index].get();
	}

	if(child != nullptr)
		return child;

	auto self = const_cast< ReflectedGroupItem * >( this );
	int skipChildren = static_cast<int>(children_.size());
	EnumerateVisibleProperties(getObject(), [&self, &child, &skipChildren](IBasePropertyPtr property, const char* groupPath)
	{
		auto groupObj = findFirstMetaData< MetaGroupObj >( *property, *self->getDefinitionManager() );
		if ( self->isSameGroup( groupObj ) && property != nullptr )
		{
			// Skip already iterated children
			if( --skipChildren < 0 )
			{
				self->children_.emplace_back( new ReflectedPropertyItem( property, self, groupPath ) );
				child = self->children_.back().get();
				return false;
			}
		}
		return true;
	});

	return child;
}

bool ReflectedGroupItem::empty() const
{
	auto self = this;
	bool isEmpty = true;
	EnumerateVisibleProperties(getObject(), [&self, &isEmpty](IBasePropertyPtr property, const char*){
		auto groupObj = findFirstMetaData< MetaGroupObj >(*property, *self->getDefinitionManager());
		if ( self->isSameGroup( groupObj ) )
		{
			isEmpty = false;
		}
		return isEmpty;
	});
	return isEmpty;
}

size_t ReflectedGroupItem::size() const
{
	auto self = this;
	size_t count = 0;
	EnumerateVisibleProperties(getObject(), [&self, &count](IBasePropertyPtr property, const char*){
		auto groupObj =	findFirstMetaData< MetaGroupObj >( *property, *self->getDefinitionManager() );
		count += self->isSameGroup( groupObj );
		return true;
	});

	return count;
}

//==============================================================================
bool ReflectedGroupItem::preSetValue(
	const PropertyAccessor & accessor, const Variant & value )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preSetValue( accessor, value ))
		{
			return true;
		}
	}
	return false;
}


//==============================================================================
bool ReflectedGroupItem::postSetValue(
	const PropertyAccessor & accessor, const Variant & value )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postSetValue( accessor, value ))
		{
			return true;
	}
	}
	return false;
}

bool ReflectedGroupItem::preItemsInserted( const PropertyAccessor & accessor, 
										   const Collection::ConstIterator & pos, size_t count )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preItemsInserted( accessor, pos, count ))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedGroupItem::postItemsInserted( const PropertyAccessor & accessor, 
											const Collection::ConstIterator & begin,
											const Collection::ConstIterator & end )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postItemsInserted( accessor, begin, end ))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedGroupItem::preItemsRemoved( const PropertyAccessor & accessor,
										  const Collection::ConstIterator & begin, const Collection::ConstIterator & end )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preItemsRemoved( accessor, begin, end ))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedGroupItem::postItemsRemoved( const PropertyAccessor & accessor,
										   const Collection::ConstIterator & pos, size_t count )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postItemsRemoved( accessor, pos, count ))
		{
			return true;
		}
	}
	return false;
}