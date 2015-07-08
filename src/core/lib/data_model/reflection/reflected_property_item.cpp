#include "reflected_property_item.hpp"
#include "reflected_object_item.hpp"
#include "reflected_enum_model.hpp"
#include "class_definition_model.hpp"

#include "data_model/generic_tree_model.hpp"
#include "data_model/i_item_role.hpp"
#include "reflection/interfaces/i_base_property.hpp"
#include "reflection/interfaces/i_reflection_property_setter.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"

#include "string_utils/string_utils.hpp"
#include <memory>
#include <codecvt>

ReflectedPropertyItem::ReflectedPropertyItem( IBaseProperty * property, ReflectedItem * parent )
	: ReflectedItem( parent, parent->getPath() + property->getName() )
{
	const MetaDisplayNameObj * displayName =
		findFirstMetaData< MetaDisplayNameObj >( property );
	if (displayName == nullptr)
	{
		displayName_ = property->getName();
		return;
	}
	std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
	displayName_ = conversion.to_bytes( displayName->getDisplayName() );
}

ReflectedPropertyItem::ReflectedPropertyItem( const std::string & propertyName, ReflectedItem * parent )
	: ReflectedItem( parent, parent->getPath() + propertyName )
	, displayName_( propertyName )
{
}

ReflectedPropertyItem::~ReflectedPropertyItem()
{
}

const char * ReflectedPropertyItem::getDisplayText( int column ) const
{
	switch (column)
	{
	case 0:
		return displayName_.c_str();

	case 1:
		return "Reflected Property";

	default:
		assert( false );
		return "";
	}
}

ThumbnailData ReflectedPropertyItem::getThumbnail( int column ) const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (findFirstMetaData< MetaThumbnailObj >( propertyAccessor ) == nullptr)
	{
		return nullptr;
	}

	ThumbnailData thumbnail;
	Variant value =  propertyAccessor.getValue();
	bool ok = false;
	ok = value.tryCast( thumbnail );
	assert( ok );
	return thumbnail;
}

Variant ReflectedPropertyItem::getData( int column, size_t roleId ) const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (roleId == ValueTypeRole::roleId_)
	{
		return propertyAccessor.getType().getName();
	}
	else if (roleId == IsEnumRole::roleId_)
	{
		return findFirstMetaData< MetaEnumObj >( propertyAccessor ) != nullptr;
	}
	else if (roleId == IsThumbnailRole::roleId_)
	{
		return findFirstMetaData< MetaThumbnailObj >( propertyAccessor ) != nullptr;
	}
	else if (roleId == IsSliderRole::roleId_)
	{
		return findFirstMetaData< MetaSliderObj >( propertyAccessor ) != nullptr;
	}
	else if (roleId == IsColorRole::roleId_)
	{
		return findFirstMetaData< MetaColorObj >( propertyAccessor ) != nullptr;
	}
	else if (roleId == ValueRole::roleId_)
	{
		return propertyAccessor.getValue();
	}
	else if (roleId == MinValueRole::roleId_)
	{
		auto minMaxObj =
			findFirstMetaData< MetaMinMaxObj >( propertyAccessor );
		if( minMaxObj == nullptr)
		{
			return Variant();
		}
		
		return minMaxObj->getMin();
	}
	else if (roleId == MaxValueRole::roleId_)
	{
		auto minMaxObj =
			findFirstMetaData< MetaMinMaxObj >( propertyAccessor );
		if( minMaxObj == nullptr)
		{
			return Variant();
		}

		return minMaxObj->getMax();
	}
	else if (roleId == EnumModelRole::roleId_)
	{
		auto enumObj = findFirstMetaData< MetaEnumObj >( propertyAccessor );
		if (enumObj)
		{
			if (getObject().isValid() == false )
			{
				return Variant();
			}
			IListModel * enumModel = new ReflectedEnumModel(
				propertyAccessor, enumObj );
			enumModel_ = std::unique_ptr< IListModel >( enumModel );
			return ObjectHandle( enumModel );
		}
	}
	else if (roleId == DefinitionRole::roleId_)
	{
		auto variant = propertyAccessor.getValue();
		ObjectHandle provider;
		variant.tryCast( provider );
		auto definition = const_cast< IClassDefinition * >(
			provider.isValid() ? provider.getDefinition() : nullptr );
		return ObjectHandle( definition );
	}
	else if (roleId == DefinitionModelRole::roleId_)
	{
		if(ReflectionUtilities::isPolyStruct( propertyAccessor ))
		{
			auto definition = propertyAccessor.getStructDefinition();
			IListModel * definitionModel = new ClassDefinitionModel( definition );
			definitionModel_ = std::unique_ptr< IListModel >( definitionModel );
			return ObjectHandle( definitionModel );
		}
	}
	return Variant();
}

bool ReflectedPropertyItem::setData( int column, size_t roleId, const Variant & data )
{
	auto propertySetter = getPropertySetter();
	if (propertySetter == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (roleId == ValueRole::roleId_)
	{
		propertySetter->setDataValue( propertyAccessor, data );
		return true;
	}
	else if (roleId == DefinitionRole::roleId_)
	{
		if(ReflectionUtilities::isPolyStruct( propertyAccessor ))
		{
			propertySetter->setDataValue( propertyAccessor, data );
			return true;
		}
	}
	return false;
}

GenericTreeItem * ReflectedPropertyItem::getChild( size_t index ) const
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

	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	Collection collection;
	bool isCollection = propertyAccessor.getValue().tryCast( collection );
	if (isCollection)
	{
		size_t i = 0;
		auto it = collection.begin();

		for (; i < index && it != collection.end(); ++it)
		{
			++i;
		}

		if (it == collection.end())
		{
			return nullptr;
		}

		// FIXME: Change to actually get the proper key type
		size_t key;
		it.key().tryCast( key );

		char buffer[ 65535 ];
		sprintf_s( buffer, sizeof(buffer), "[%d]", static_cast< int >( key ) );

		child = new ReflectedPropertyItem( buffer,
			const_cast< ReflectedPropertyItem * >( this ) );
		children_[index] = std::unique_ptr< ReflectedItem >( child );
		return child;
	}


	auto defManager
		= propertyAccessor.getDefinitionManager();
	if (defManager == nullptr)
	{
		return nullptr;
	}

	auto value = propertyAccessor.getValue();
	ObjectHandle baseProvider;
	value.tryCast( baseProvider );
	if (baseProvider.getBase< void >() == nullptr)
	{
		return nullptr;
	}
	child = new ReflectedObjectItem( baseProvider , 
		const_cast< ReflectedPropertyItem * >( this ) );
	child->hidden( true );
	children_[index] = std::unique_ptr< ReflectedItem >( child );
	return child;
}

bool ReflectedPropertyItem::empty() const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	const Variant & value = propertyAccessor.getValue();
	const bool isCollection = value.typeIs< Collection >();
	if (isCollection)
	{
		return false;
	}

	ObjectHandle handle;
	bool isObjectHandle = value.tryCast( handle );
	if(isObjectHandle)
	{
		auto def = handle.getDefinition();
		if(def != nullptr)
		{
			return false;
		}
	}

	return true;
}

size_t ReflectedPropertyItem::size() const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	Collection collection;
	const Variant & value = propertyAccessor.getValue();
	bool isCollection = value.tryCast( collection );
	if (isCollection)
	{
		auto count = collection.size();
		return count > 0 ? count : 1;
	}

	ObjectHandle handle;
	bool isObjectHandle = value.tryCast( handle );
	if(isObjectHandle)
	{
		auto def = handle.getDefinition();
		if(def != nullptr)
		{
			return 1;
		}
	}

	return 0;
}


//==============================================================================
bool ReflectedPropertyItem::preSetValue(
	const PropertyAccessor & accessor, const Variant & value )
{
	auto obj = getObject();
	auto otherObj = accessor.getRootObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{

		ObjectHandle handle;
		bool isObjectHandle = value.tryCast( handle );
		if(isObjectHandle)
		{
			auto def = handle.getDefinition();
			if(def != nullptr)
			{
				getModel()->notifyPreDataChanged( this, 1, DefinitionRole::roleId_,
					value );
				return true;
			}
		}
		getModel()->notifyPreDataChanged( this, 1, ValueRole::roleId_,
			value );
		return true;
	}

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
bool ReflectedPropertyItem::postSetValue(
	const PropertyAccessor & accessor, const Variant & value )
{
	auto obj = getObject();
	auto otherObj = accessor.getRootObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		ObjectHandle handle;
		bool isObjectHandle = value.tryCast( handle );
		if(isObjectHandle)
		{
			auto def = handle.getDefinition();
			if(def != nullptr)
			{
				children_.clear();
				getModel()->notifyPostDataChanged( this, 1, DefinitionRole::roleId_,
					value );
				return true;
			}
		}
		getModel()->notifyPostDataChanged( this, 1, ValueRole::roleId_,
			value );
		return true;
	}

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

bool ReflectedPropertyItem::preItemsInserted( const PropertyAccessor & accessor, 
										  const Collection::ConstIterator & pos, size_t count )
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (accessor.getProperty() == propertyAccessor.getProperty())
	{
		Collection collection;
		propertyAccessor.getValue().tryCast( collection );
		auto it = collection.begin();
		
		size_t index = 0;
		for (; it != pos; assert( it != collection.end() ), ++it, ++index) {}

		getModel()->notifyPreItemsInserted( this, index, count );
		return true;
	}

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

bool ReflectedPropertyItem::postItemsInserted( const PropertyAccessor & accessor, 
										   const Collection::ConstIterator & begin, const Collection::ConstIterator & end )
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (accessor.getProperty() == propertyAccessor.getProperty())
	{
		Collection collection;
		propertyAccessor.getValue().tryCast( collection );
		auto it = collection.begin();

		size_t index = 0;
		for (; it != begin; assert( it != collection.end() ), ++it, ++index) {}

		size_t count = 0;
		for (; it != end; assert( it != collection.end() ), ++it, ++count) {}

		auto insertIt = children_.begin();
		for (size_t i = 0; i < index && insertIt != children_.end(); ++i, ++insertIt) {}

		for (size_t i = 0; i < count; ++i)
		{
			insertIt = children_.emplace( insertIt, nullptr );
		}

		getModel()->notifyPostItemsInserted( this, index, count );
		return true;
	}

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

bool ReflectedPropertyItem::preItemsRemoved( const PropertyAccessor & accessor,
										 const Collection::ConstIterator & begin, const Collection::ConstIterator & end )
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (accessor.getProperty() == propertyAccessor.getProperty())
	{
		Collection collection;
		propertyAccessor.getValue().tryCast( collection );
		auto it = collection.begin();

		size_t index = 0;
		for (; it != begin; assert( it != collection.end() ), ++it, ++index) {}

		size_t count = 0;
		for (; it != end; assert( it != collection.end() ), ++it, ++count) {}

		getModel()->notifyPreItemsRemoved( this, index, count );
		return true;
	}

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

bool ReflectedPropertyItem::postItemsRemoved( const PropertyAccessor & accessor,
										  const Collection::ConstIterator & pos, size_t count )
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (accessor.getProperty() == propertyAccessor.getProperty())
	{
		Collection collection;
		propertyAccessor.getValue().tryCast( collection );
		auto it = collection.begin();

		size_t index = 0;
		for (; it != pos; assert( it != collection.end() ), ++it, ++index) {}

		auto eraseBeginIt = children_.begin();
		for (size_t i = 0; i < index && eraseBeginIt != children_.end(); ++i, ++eraseBeginIt) {}

		auto eraseEndIt = eraseBeginIt;
		for (size_t i = 0; i < count && eraseEndIt != children_.end(); ++i, ++eraseEndIt) {}

		children_.erase( eraseBeginIt, eraseEndIt );

		getModel()->notifyPostItemsRemoved( this, index, count );
		return true;
	}

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