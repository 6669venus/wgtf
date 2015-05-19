#include "reflected_object_item.hpp"
#include "reflected_group_item.hpp"
#include "reflected_property_item.hpp"

#include "reflection/interfaces/i_base_property.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "string_utils/string_utils.hpp"

#include <codecvt>

ReflectedObjectItem::ReflectedObjectItem( const ObjectHandle & object, ReflectedItem * parent )
	: ReflectedItem( parent, parent ? parent->getPath() + "." : "" )
	, object_( object )
{
	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return;
	}

	const MetaDisplayNameObj * displayName =
		findFirstMetaData< MetaDisplayNameObj >( *definition );
	if (displayName == nullptr)
	{
		displayName_ = definition->getName();
		return;
	}

	std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
	displayName_ = conversion.to_bytes( displayName->getDisplayName() );
}

const IClassDefinition * ReflectedObjectItem::getDefinition() const 
{ 
	return object_.getDefinition();
}


const char * ReflectedObjectItem::getDisplayText( int column ) const
{
	switch (column)
	{
	case 0:
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

	if (property != nullptr)
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