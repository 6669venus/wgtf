#include "reflected_enum_model_new.hpp"

#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_string_utils/string_utils.hpp"

#include <codecvt>
#include <iterator>

namespace
{
	class ReflectedEnumItem : public AbstractListItem
	{
	public:
		ReflectedEnumItem( int index, const std::string & text ) 
			: index_( index )
			, text_( text ) 
		{}

		//const char * getDisplayText( int column ) const 
		//{ 
		//	return text_.c_str();
		//}

		//ThumbnailData getThumbnail( int column ) const
		//{
		//	return nullptr;
		//}

		Variant getData( int column, size_t roleId ) const override
		{ 
			if (roleId == ValueRole::roleId_)
			{
				return Variant( index_ );
			}
			else if (roleId == IndexPathRole::roleId_)
			{
				return text_ + std::to_string( index_ );
			}
			return Variant();
		}
		
		bool setData( int column, size_t roleId, const Variant & data ) override
		{ 
			return false;
		}

	private:
		int index_;
		std::string text_;
	};
}

ReflectedEnumModelNew::ReflectedEnumModelNew( const PropertyAccessor & pA, const MetaEnumObj * enumObj )
{
	std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
	const wchar_t * enumString = enumObj->getEnumString();
	if (enumString != nullptr)
	{
		int index = 0;
		const wchar_t * start = enumString;
		const wchar_t * enumStringEnd = start + wcslen( start );
		while (start < enumStringEnd)
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

			items_.push_back( new ReflectedEnumItem( index, conversion.to_bytes( text ) ) );
			start = trueEnd + 1;
			++index;
		}
		return;
	}

	auto value = pA.getParent().getValue();
	ObjectHandle baseProvider;
	value.tryCast( baseProvider );
	Collection collection = enumObj->generateEnum( baseProvider, *pA.getDefinitionManager() );
	auto it = collection.begin();
	auto itEnd = collection.end();
	for (; it != itEnd; ++it)
	{
		int index;
		it.key().tryCast( index );
		Variant itValue = it.value();
		std::string text;
		itValue.tryCast( text );
		items_.push_back( new ReflectedEnumItem( index, text ) );
	}
}

ReflectedEnumModelNew::~ReflectedEnumModelNew()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
	{
		delete *it;
	}
}

AbstractItem * ReflectedEnumModelNew::item( int row ) const
{
	assert( row >= 0 );
	const auto index = static_cast< std::vector< AbstractItem * >::size_type >( row );
	assert( index < items_.size() );
	return items_[ index ];
}

int ReflectedEnumModelNew::index( const AbstractItem * item ) const
{
	auto it = std::find( items_.begin(), items_.end(), item );
	assert( it != items_.end() );
	return static_cast< int >( std::distance( items_.begin(), it ) );
}


int ReflectedEnumModelNew::rowCount() const
{
	return static_cast< int >( items_.size() );
}


int ReflectedEnumModelNew::columnCount() const
{
	return 1;
}
