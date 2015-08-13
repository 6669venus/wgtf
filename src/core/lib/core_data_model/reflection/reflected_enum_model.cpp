#include "reflected_enum_model.hpp"
#include "reflected_item.hpp"

#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_string_utils/string_utils.hpp"
#include <codecvt>

namespace
{
	class ReflectedEnumItem : public IItem
	{
	public:
		ReflectedEnumItem( int index, const std::string & text, const Variant & data ) 
			: index_( index )
			, text_( text ) 
			, data_( data )
		{}

		int columnCount() const 
		{ 
			return 1; 
		}

		const char * getDisplayText( int column ) const 
		{ 
			return text_.c_str(); 
		}

		ThumbnailData getThumbnail( int column ) const
		{
			return nullptr;
		}

		Variant getData( int column, size_t roleId ) const 
		{ 
			if (roleId == ValueRole::roleId_)
			{
				return data_;
			}
			return Variant();
		}
		
		bool setData( int column, size_t roleId, const Variant & data ) 
		{ 
			return false; 
		}

	private:
		friend ReflectedEnumModel;
		int index_;
		std::string text_;
		Variant data_;
	};
}

ReflectedEnumModel::ReflectedEnumModel( const PropertyAccessor & pA, const MetaEnumObj * enumObj )
{
	std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
	const wchar_t * enumString = enumObj->getEnumString();
	if (enumString != nullptr)
	{
		int index = 0;
		const wchar_t * start = enumString;
		const wchar_t * enumStringEnd = start + wcslen( start );
		const wchar_t * end = nullptr;
		while( start < enumStringEnd )
		{
			const wchar_t * end = nullptr;
			end = wcsstr( start, L"|" );
			if (end == nullptr)
			{
				end = start + wcslen( start );
			}
			const wchar_t * trueEnd = end;
			const wchar_t * indexStart = wcsstr( start, L"=" );
			if (indexStart != nullptr &&
				indexStart <= end )
			{
				index =  static_cast<int>( wcstol( indexStart + 1, nullptr, 10 ) );
				end = indexStart;
			}
			std::wstring text( start, end );

			items_.push_back( new ReflectedEnumItem( index, conversion.to_bytes( text ), index ) );
			start = trueEnd + 1;
			++index;
		}
		return;
	}

	auto value = pA.getParent().getValue();
	ObjectHandle baseProvider;
	value.tryCast( baseProvider );
	Collection collection = enumObj->generateEnum( baseProvider );
	auto it = collection.begin();
	auto itEnd = collection.end();
	for( ; it != itEnd; ++it )
	{
		int index;
		it.key().tryCast( index );
		Variant itValue = it.value();
		std::string text;
		itValue.tryCast(text);
		items_.push_back(new ReflectedEnumItem(index, text, index));
	}
}

ReflectedEnumModel::~ReflectedEnumModel()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
	{
		delete *it;
	}
}

IItem * ReflectedEnumModel::item( size_t index ) const
{
	assert( index < items_.size() );
	return items_[ index ];
}

size_t ReflectedEnumModel::index( const IItem * item ) const
{
	auto it = std::find( items_.begin(), items_.end(), item );
	assert( it != items_.end() );
	return it - items_.begin();
}

IItem * ReflectedEnumModel::findItemByData(const Variant & data) const
{
	for (auto item : items_)
	{
		assert( item != nullptr );
		Variant value = item->getData( 0, ValueRole::roleId_ );
		if (data == value)
		{
			return item;
		}
	}
	return nullptr;
}


bool ReflectedEnumModel::empty() const
{
	return items_.empty();
}


size_t ReflectedEnumModel::size() const
{
	return items_.size();
}