#include "reflected_object_item.hpp"
#include "reflected_group_item.hpp"
#include "reflected_property_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/generic_tree_model.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"

#include <codecvt>

ReflectedObjectItem::ReflectedObjectItem( const ObjectHandle & object, ReflectedItem * parent )
	: ReflectedItem( parent, parent ? parent->getPath() + "." : "" )
	, object_( object )
{
}

const IClassDefinition * ReflectedObjectItem::getDefinition() const 
{ 
	return object_.getDefinition( *getDefinitionManager() );
}


const char * ReflectedObjectItem::getDisplayText( int column ) const
{
	switch (column)
	{
	case 0:
		if (displayName_.empty())
		{
			auto definition = getDefinition();
			if (definition == nullptr)
			{
				return nullptr;
			}
			const MetaDisplayNameObj * displayName =
				findFirstMetaData< MetaDisplayNameObj >( *definition );
			if (displayName == nullptr)
			{
				displayName_ = definition->getName();
			}
			else
			{
				std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
				displayName_ = conversion.to_bytes( displayName->getDisplayName() );
			}
		}
		return displayName_.c_str();

	case 1:
		{
			auto definition = getDefinition();
			if (definition == nullptr)
			{
				return "";
			}
			return definition->getName();
		}

	default:
		assert( false );
		return "";
	}
}


Variant ReflectedObjectItem::getData( int column, size_t roleId ) const
{
	assert( parent_ == nullptr );
	if (roleId == ValueRole::roleId_)
	{
		return object_;
	}

	if (roleId == IndexPathRole::roleId_)
	{
		if (parent_ == nullptr)
		{
			return "";
		}
		return this->getPath();
	}

	return Variant();
}

bool ReflectedObjectItem::setData(int column, size_t roleId, const Variant & data)
{
	if (parent_ != nullptr)
	{
		return false;
	}
	return false;
}


GenericTreeItem * ReflectedObjectItem::getChild( size_t index ) const
{
	if (children_.size() <= index )
	{
		children_.reserve( index + 1 );
		while (children_.size() <= index)
		{
			children_.emplace_back( nullptr );
		}
	}

	auto child = children_[index].get();
	if (child != NULL)
	{
		return child;
	}

	size_t i = 0;

	IBaseProperty * property = nullptr;
	const MetaGroupObj * groupObj = nullptr;

	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return nullptr;
	}

	auto properties = definition->allProperties();
	auto it = properties.begin();

	std::set< std::wstring > groups;
	for (; i <= index && it != properties.end(); ++it)
	{
		property = it.current();
		groupObj = findFirstMetaData< MetaGroupObj >( property );
		if (groupObj == nullptr ||
			!groups.insert( groupObj->getGroupName() ).second)
		{
			continue;
		}
		++i;
	}

	if (i > index)
	{
		child = new ReflectedGroupItem( groupObj, 
			const_cast< ReflectedObjectItem * >( this ) );
		children_[index] = std::unique_ptr< ReflectedItem >( child );
		return child;
	}

	it = properties.begin();
	for (; i <= index && it != properties.end(); ++it)
	{
		property = it.current();
		groupObj = findFirstMetaData< MetaGroupObj >( property );
		if (groupObj != nullptr)
		{
			continue;
		}
		++i;
	}

	if ((property != nullptr) && (!property->isMethod()))
	{
		child = new ReflectedPropertyItem( property, 
			const_cast< ReflectedObjectItem * >( this ) );
		children_[index] = std::unique_ptr< ReflectedItem >( child );
		return child;
	}

	return nullptr;
}

bool ReflectedObjectItem::empty() const
{
	return false;
}

size_t ReflectedObjectItem::size() const
{
	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return 1;
	}

	auto properties = definition->allProperties();
	size_t count = 0;

	std::set< std::wstring > groups;
	for (auto it = properties.begin(); it != properties.end(); ++it)
	{
		auto property = it.current();
		if (property->isMethod())
		{
			continue;
		}
		auto groupObj =	findFirstMetaData< MetaGroupObj >( property );
		if (groupObj != nullptr &&
			!groups.insert( groupObj->getGroupName() ).second)
		{
			continue;
		}
		++count;
	}

	// always return at least one child
	return count > 0 ? count : 1;
}


//==============================================================================
bool ReflectedObjectItem::preSetValue(
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
bool ReflectedObjectItem::postSetValue(
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

bool ReflectedObjectItem::preItemsInserted( const PropertyAccessor & accessor, 
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

bool ReflectedObjectItem::postItemsInserted( const PropertyAccessor & accessor, 
	const Collection::ConstIterator & begin, const Collection::ConstIterator & end )
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

bool ReflectedObjectItem::preItemsRemoved( const PropertyAccessor & accessor,
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

bool ReflectedObjectItem::postItemsRemoved( const PropertyAccessor & accessor,
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