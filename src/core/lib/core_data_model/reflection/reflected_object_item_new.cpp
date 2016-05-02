#include "reflected_object_item_new.hpp"
#include "reflected_group_item_new.hpp"
#include "reflected_property_item_new.hpp"

#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_string_utils/string_utils.hpp"

#include <codecvt>
#include <set>


namespace
{

bool compareWStrings( const wchar_t * a, const wchar_t * b )
{
	return wcscmp( a, b ) < 0;
}

} // namespace


class ReflectedObjectItemNew::Implementation
{
public:
	Implementation( IComponentContext & contextManager,
		const ObjectHandle & object );

	typedef std::set< const wchar_t *, bool(*)( const wchar_t *, const wchar_t * ) > Groups;

	const Groups & getGroups( const ReflectedObjectItemNew & self );

	IComponentContext & contextManager_;
	ObjectHandle object_;
	std::string displayName_;
	std::vector< std::unique_ptr< ReflectedTreeItemNew > > children_;
	Groups groups_;
};


ReflectedObjectItemNew::Implementation::Implementation(
	IComponentContext & contextManager,
	const ObjectHandle & object )
	: contextManager_( contextManager )
	, object_( object )
	, groups_( compareWStrings )
{
}


const ReflectedObjectItemNew::Implementation::Groups &
ReflectedObjectItemNew::Implementation::getGroups(
	const ReflectedObjectItemNew & self )
{
	auto definition = self.getDefinition();
	if (!groups_.empty() || (definition == nullptr))
	{
		return groups_;
	}

	auto pDefinitionManager = self.getDefinitionManager();
	if (pDefinitionManager == nullptr)
	{
		return groups_;
	}

	auto & parent = const_cast< ReflectedObjectItemNew & >( self );
	self.enumerateVisibleProperties( [ this, &self, &parent, pDefinitionManager ](
		const IBasePropertyPtr & property,
		const std::string & inPlacePath )
	{
		const MetaGroupObj * groupObj = findFirstMetaData< MetaGroupObj >( *property,
			*pDefinitionManager );
		if (groupObj != nullptr && groups_.insert( groupObj->getGroupName() ).second)
		{
			children_.emplace_back( new ReflectedGroupItemNew(
				contextManager_,
				groupObj,
				&parent,
				inPlacePath ) );
		}
		return true;
	} );
	return groups_;
}


ReflectedObjectItemNew::ReflectedObjectItemNew( IComponentContext & contextManager,
	const ObjectHandle & object,
	ReflectedTreeItemNew * parent )
	: ReflectedTreeItemNew( contextManager,
		parent,
		parent ? parent->getPath() + "." : "" )
	, impl_( new Implementation( contextManager, object ) )
{
}


ReflectedObjectItemNew::~ReflectedObjectItemNew()
{
}


Variant ReflectedObjectItemNew::getData( int column, size_t roleId ) const /* override */
{
	// Only works for root items?
	assert( parent_ == nullptr );
	if (roleId == ValueRole::roleId_)
	{
		return impl_->object_;
	}
	if (roleId == ValueTypeRole::roleId_)
	{
		return TypeId::getType< ObjectHandle >().getName();
	}
	else if (roleId == KeyRole::roleId_)
	{
		switch (column)
		{
		case 0:
			if (impl_->displayName_.empty())
			{
				auto definition = this->getDefinition();
				if (definition == nullptr)
				{
					return "";
				}
				auto pDefinitionManager = this->getDefinitionManager();
				if (pDefinitionManager == nullptr)
				{
					return "";
				}
				const MetaDisplayNameObj * displayName =
					findFirstMetaData< MetaDisplayNameObj >( *definition,
						*pDefinitionManager );
				if (displayName == nullptr)
				{
					impl_->displayName_ = definition->getName();
				}
				else
				{
					std::wstring_convert< Utf16to8Facet > conversion(
						Utf16to8Facet::create() );
					impl_->displayName_ = conversion.to_bytes(
						displayName->getDisplayName() );
				}
			}
			return impl_->displayName_.c_str();

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
	if (roleId == KeyTypeRole::roleId_)
	{
		return TypeId::getType< const char * >().getName();
	}
	else if (roleId == IndexPathRole::roleId_)
	{
		return this->getPath();
	}
	else if (roleId == ObjectRole::roleId_)
	{
		return this->getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return this->getRootObject();
	}

	return Variant();
}


bool ReflectedObjectItemNew::setData( int column, size_t roleId,
	const Variant & data ) /* override */
{
	if (roleId != ValueRole::roleId_)
	{
		return false;
	}

	ObjectHandle other;
	if (!data.tryCast(other))
	{
		return false;
	}

	auto pDefinitionManager = this->getDefinitionManager();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = this->getRootObject();
	auto definition = obj.getDefinition( *pDefinitionManager );
	auto otherDef = other.getDefinition( *pDefinitionManager );
	if (definition != otherDef)
	{
		return false;
	}

	for (auto prop : definition->allProperties())
	{
		auto accessor = definition->bindProperty( prop->getName(), obj );
		auto otherAccessor = definition->bindProperty( prop->getName(), other );
		if (accessor.canSetValue())
		{
			assert( otherAccessor.canGetValue() );
			accessor.setValue( otherAccessor.getValue() );
		}
	}

	const auto pParent = this->getParent();
	if (pParent)
	{
		return pParent->setData( column, roleId, obj );
	}

	return true;
}


const ObjectHandle & ReflectedObjectItemNew::getRootObject() const /* override */
{
	return parent_ ? parent_->getRootObject() : impl_->object_;
}


const ObjectHandle & ReflectedObjectItemNew::getObject() const /* override */
{
	return impl_->object_;
}


const IClassDefinition * ReflectedObjectItemNew::getDefinition() const 
{
	auto pDefinitionManager = this->getDefinitionManager();
	if (pDefinitionManager == nullptr)
	{
		return nullptr;
	}
	return impl_->object_.getDefinition( *pDefinitionManager );
}


ReflectedTreeItemNew * ReflectedObjectItemNew::getChild( size_t index ) const
{
	if (impl_->children_.size() > index)
	{
		return impl_->children_[ index ].get();
	}

	size_t currentIndex = 0;
	this->enumerateChildren( [ &currentIndex, index ]( ReflectedTreeItemNew & item )
	{
		return (currentIndex++ == index);
	} );

	if (currentIndex > 0)
	{
		return impl_->children_[--currentIndex].get();
	}
	return nullptr;
}


int ReflectedObjectItemNew::rowCount() const
{
	int count = 0;

	this->enumerateChildren( [ &count ]( ReflectedTreeItemNew & )
	{
		++count;
		return true;
	} );

	return count;
}


bool ReflectedObjectItemNew::preSetValue( const PropertyAccessor & accessor,
	const Variant & value )
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
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


bool ReflectedObjectItemNew::postSetValue( const PropertyAccessor & accessor,
	const Variant & value )
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
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


void ReflectedObjectItemNew::enumerateChildren(
	const ReflectedItemCallback & callback ) const
{
	// Get the groups and iterate them first
	const auto & groups = impl_->getGroups( *this );

	// This will iterate all the groups and any cached children
	for (auto & child : impl_->children_)
	{
		if (!callback( *child.get() ))
		{
			return;
		}
	}

	auto pDefinitionManager = this->getDefinitionManager();
	if (pDefinitionManager == nullptr)
	{
		return;
	}

	// ReflectedGroupItem children handle grouped items
	int skipChildCount = static_cast< int >( impl_->children_.size() - groups.size() );
	auto parent = const_cast< ReflectedObjectItemNew * >( this );
	this->enumerateVisibleProperties( [ & ]( const IBasePropertyPtr & property,
		const std::string & inPlacePath )
	{
		bool isGrouped = findFirstMetaData< MetaGroupObj >( *property,
			*pDefinitionManager ) != nullptr;
		if (!isGrouped)
		{
			// Skip already iterated children
			if (--skipChildCount < 0)
			{
				impl_->children_.emplace_back( new ReflectedPropertyItemNew(
					impl_->contextManager_,
					property,
					parent,
					inPlacePath ) );
				return callback( *impl_->children_.back().get() );
			}
		}
		return true;
	} );
}
