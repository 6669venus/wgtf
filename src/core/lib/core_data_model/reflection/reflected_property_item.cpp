#include "reflected_property_item.hpp"
#include "reflected_object_item.hpp"
#include "reflected_enum_model.hpp"
#include "class_definition_model.hpp"

#include "core_data_model/generic_tree_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/i_combobox_list_model.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"

#include "core_string_utils/string_utils.hpp"
#include <memory>
#include <codecvt>
#include <limits>

namespace
{
	struct MaxMinValuePair
	{
		MaxMinValuePair( const Variant & min, const Variant & max )
			: minValue_( min )
			, maxValue_( max )
		{
			
		}
		Variant minValue_;
		Variant maxValue_;
	};
	
	template <typename T>
	struct MaxMinValuePairT
		: MaxMinValuePair
	{
		typedef MaxMinValuePair base;
		typedef T value_type;
		MaxMinValuePairT( const T & min, const T & max )
			: base( min, max )
		{
		}
	};

	typedef std::unordered_map<const TypeId, MaxMinValuePair> MaxMinValuePairMap;

	const MaxMinValuePairMap & getValuePairMap()
	{
		static MaxMinValuePairMap valueMap;
		if(valueMap.empty())
		{
			static TypeId int8Type = TypeId::getType<int8_t>();
			static TypeId int16Type = TypeId::getType<int16_t>();
			static TypeId int32Type = TypeId::getType<int32_t>();
			static TypeId int64Type = TypeId::getType<int64_t>();
			static TypeId uint8Type = TypeId::getType<uint8_t>();
			static TypeId uint16Type = TypeId::getType<uint16_t>();
			static TypeId uint32Type = TypeId::getType<uint32_t>();
			static TypeId uint64Type = TypeId::getType<uint64_t>();
			static TypeId longType = TypeId::getType<long>();
			static TypeId ulongType = TypeId::getType<unsigned long>();
			static TypeId floatType = TypeId::getType<float>();
			static TypeId doubleType = TypeId::getType<double>();

			valueMap.emplace( 
				std::make_pair( int8Type, 
					MaxMinValuePairT<int8_t>( std::numeric_limits<int8_t>::min(), 
											  std::numeric_limits<int8_t>::max() )));
			valueMap.emplace( 
				std::make_pair( int16Type, 
					MaxMinValuePairT<int16_t>( std::numeric_limits<int16_t>::min(), 
											   std::numeric_limits<int16_t>::max() )));
			valueMap.emplace( 
				std::make_pair( int32Type, 
					MaxMinValuePairT<int32_t>( std::numeric_limits<int32_t>::min(), 
											   std::numeric_limits<int32_t>::max() )));
			valueMap.emplace( 
				std::make_pair( int64Type, 
					MaxMinValuePairT<int64_t>( std::numeric_limits<int64_t>::min(), 
											   std::numeric_limits<int64_t>::max() )));
			valueMap.emplace( 
				std::make_pair( uint8Type, 
					MaxMinValuePairT<uint8_t>( std::numeric_limits<uint8_t>::min(), 
											   std::numeric_limits<uint8_t>::max() )));
			valueMap.emplace( 
				std::make_pair( uint16Type, 
					MaxMinValuePairT<uint16_t>( std::numeric_limits<uint16_t>::min(), 
												std::numeric_limits<uint16_t>::max() )));
			valueMap.emplace( 
				std::make_pair( uint32Type, 
					MaxMinValuePairT<uint32_t>( std::numeric_limits<uint32_t>::min(), 
												std::numeric_limits<uint32_t>::max() )));
			valueMap.emplace( 
				std::make_pair( uint64Type, 
					MaxMinValuePairT<uint64_t>( std::numeric_limits<uint64_t>::min(), 
											    std::numeric_limits<uint64_t>::max() )));
			valueMap.emplace( 
				std::make_pair( longType, 
					MaxMinValuePairT<int32_t>( std::numeric_limits<int32_t>::min(), 
										    std::numeric_limits<int32_t>::max() )));
			valueMap.emplace( 
				std::make_pair( ulongType, 
					MaxMinValuePairT<uint32_t>( std::numeric_limits<uint32_t>::min(), 
													 std::numeric_limits<uint32_t>::max() )));
			valueMap.emplace( 
				std::make_pair( floatType, 
					MaxMinValuePairT<float>( std::numeric_limits<float>::min(), 
											 std::numeric_limits<float>::max() )));
			valueMap.emplace( 
				std::make_pair( doubleType, 
					MaxMinValuePairT<double>( std::numeric_limits<double>::min(), 
											  std::numeric_limits<double>::max() )));
		}
		return valueMap;
	}

	Variant getMaxValue( const TypeId & typeId )
	{
		const MaxMinValuePairMap valuePairMap = getValuePairMap();
		auto findIt = valuePairMap.find( typeId );
		if( findIt != valuePairMap.end())
		{
			return findIt->second.maxValue_;
		}
		return Variant();
	}

	Variant getMinValue( const TypeId & typeId )
	{
		const MaxMinValuePairMap valuePairMap = getValuePairMap();
		auto findIt = valuePairMap.find( typeId );
		if( findIt != valuePairMap.end())
		{
			return findIt->second.minValue_;
		}
		return Variant();
	}
}

ReflectedPropertyItem::ReflectedPropertyItem( IBaseProperty * property, ReflectedItem * parent )
	: ReflectedItem( parent, parent->getPath() + property->getName() )
	, currentIndex_( -1 )
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
	, currentIndex_(-1)
{
}

ReflectedPropertyItem::~ReflectedPropertyItem()
{
	currentIndex_.onPostDataChanged().remove< ReflectedPropertyItem,
		&ReflectedPropertyItem::onPostDataChanged >(const_cast<ReflectedPropertyItem*>(this));
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
		if( minMaxObj != nullptr)
		{
			return minMaxObj->getMin();
		}
		else
		{
			TypeId typeId = propertyAccessor.getType();
			return getMinValue( typeId );
		}
	}
	else if (roleId == MaxValueRole::roleId_)
	{
		auto minMaxObj =
			findFirstMetaData< MetaMinMaxObj >( propertyAccessor );
		if( minMaxObj != nullptr)
		{
			return minMaxObj->getMax();
		}
		else
		{
			TypeId typeId = propertyAccessor.getType();
			return getMaxValue( typeId );
		}
	}
	else if (roleId == ModelValueRole::roleId_)
	{
		return ObjectHandle(&currentIndex_);
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
			if (listModel_ == nullptr)
			{
				IComboBoxListModel * enumModel = new ReflectedEnumModel(
					propertyAccessor, enumObj);
				listModel_ = std::unique_ptr< IComboBoxListModel >(enumModel);
				Variant value = propertyAccessor.getValue();
				auto item = listModel_->findItemByData(value);
				currentIndex_.onPostDataChanged().remove< ReflectedPropertyItem,
					&ReflectedPropertyItem::onPostDataChanged >(const_cast<ReflectedPropertyItem*>(this));
				if (item != nullptr)
				{
					currentIndex_.value(static_cast<int>(listModel_->index(item)));
				}
				currentIndex_.onPostDataChanged().add< ReflectedPropertyItem,
					&ReflectedPropertyItem::onPostDataChanged >(const_cast<ReflectedPropertyItem*>(this));
			}
			return ObjectHandle( listModel_.get() );
		}
	}
	else if (roleId == DefinitionModelRole::roleId_)
	{
		if(ReflectionUtilities::isPolyStruct( propertyAccessor ))
		{
			if (listModel_ == nullptr)
			{
				auto definition = propertyAccessor.getStructDefinition();
				IComboBoxListModel * definitionModel = new ClassDefinitionModel(definition);
				listModel_ = std::unique_ptr< IComboBoxListModel >(definitionModel);
				Variant value = propertyAccessor.getValue();
				auto item = listModel_->findItemByData(value);
				currentIndex_.onPostDataChanged().remove< ReflectedPropertyItem,
					&ReflectedPropertyItem::onPostDataChanged >(const_cast<ReflectedPropertyItem*>(this));
				if (item != nullptr)
				{
					currentIndex_.value(static_cast<int>(listModel_->index(item)));
				}
				currentIndex_.onPostDataChanged().add< ReflectedPropertyItem,
					&ReflectedPropertyItem::onPostDataChanged >(const_cast<ReflectedPropertyItem*>(this));
			}
			return ObjectHandle( listModel_.get() );
		}
	}
	return Variant();
}

bool ReflectedPropertyItem::setData( int column, size_t roleId, const Variant & data )
{
	auto controller = getController();
	if (controller == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty( 
		path_.c_str(), obj );

	if (roleId == ValueRole::roleId_ || 
		roleId == ModelValueRole::roleId_)
	{
		controller->setValue( propertyAccessor, data );
		return true;
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

		std::string s = "[" + std::to_string(static_cast< int >( key )) + "]";

		child = new ReflectedPropertyItem( s,
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
		if (listModel_ != nullptr)
		{
			getModel()->notifyPreDataChanged(this, 1, ModelValueRole::roleId_,
				value);
			return true;
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
		if (listModel_ != nullptr)
		{
			children_.clear();
			getModel()->notifyPostDataChanged(this, 1, ModelValueRole::roleId_,
				value);
			auto item = listModel_->findItemByData(value);
			if (item == nullptr)
			{
				currentIndex_.value(-1);
			}
			else
			{
				currentIndex_.value(static_cast<int>(listModel_->index(item)));
			}
			return true;
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

void ReflectedPropertyItem::onPostDataChanged(const IValueChangeNotifier* sender,
											  const IValueChangeNotifier::PostDataChangedArgs& args)
{
	auto controller = getController();
	if (controller == nullptr)
	{
		return;
	}
	if (listModel_ == nullptr)
	{
		return;
	}
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition()->bindProperty(
		path_.c_str(), obj);
	Variant oldValue = propertyAccessor.getValue();
	auto oldItem = listModel_->findItemByData(oldValue);
	Variant value(ObjectHandle(nullptr));
	IItem* item = nullptr;
	if ((currentIndex_.value() >= 0) && (currentIndex_.value() < static_cast<int>( listModel_->size() )))
	{
		item = listModel_->item(currentIndex_.value());
	}
	
	if (oldItem == item)
	{
		return;
	}
	if (item != nullptr)
	{
		value = item->getData(1, ValueRole::roleId_);
	}

	controller->setValue(propertyAccessor, value);
}