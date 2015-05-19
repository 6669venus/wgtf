#include "reflected_group_item.hpp"
#include "reflected_property_item.hpp"

#include "reflection/interfaces/i_base_property.hpp"
#include "reflection/interfaces/i_class_definition.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"

#include "string_utils/string_utils.hpp"
#include <codecvt>

ReflectedGroupItem::ReflectedGroupItem( const MetaGroupObj * groupObj, ReflectedItem * parent )
	: ReflectedItem( parent, parent->getPath() )
	, groupObj_( groupObj )
{
	std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
	if (groupObj_ == nullptr)
	{
		displayName_.clear();
	}
	displayName_ = conversion.to_bytes( groupObj_->getGroupName() );
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

GenericTreeItem * ReflectedGroupItem::getChild( size_t index ) const
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

	for (; i <= index && it != properties.end(); ++it)
	{
		property = it.current();
		groupObj = findFirstMetaData< MetaGroupObj >( property );
		if (groupObj == nullptr ||
			(groupObj != groupObj_ && wcscmp(groupObj->getGroupName(), groupObj_->getGroupName()) != 0))
		{
			continue;
		}
		++i;
	}

	if (property != nullptr)
	{
		child = new ReflectedPropertyItem( property, 
			const_cast< ReflectedGroupItem * >( this ) );
		children_[index] = std::unique_ptr< ReflectedItem >( child );
		return child;
	}

	return nullptr;
}

bool ReflectedGroupItem::empty() const
{
	return false;
}

size_t ReflectedGroupItem::size() const
{
	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return 1;
	}

	size_t count = 0;

	auto properties = definition->allProperties();
	for (auto it = properties.begin(); it != properties.end(); ++it)
	{
		auto property = it.current();
		auto groupObj =	findFirstMetaData< MetaGroupObj >( property );
		if (groupObj == nullptr ||
			(groupObj != groupObj_ && wcscmp(groupObj->getGroupName(), groupObj_->getGroupName()) != 0))
		{
			continue;
		}
		++count;
	}

	// always return at least one child
	return count > 0 ? count : 1;
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