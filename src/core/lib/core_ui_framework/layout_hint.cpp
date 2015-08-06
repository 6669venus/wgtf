#include "layout_hint.hpp"
#include "layout_tags.hpp"

#include <algorithm>
#include <vector>

LayoutHint::LayoutHint()
{

}

LayoutHint::LayoutHint( const char * hint, float strength )
{
	hints_.insert( std::make_pair( std::string( hint ), strength ) );
}

LayoutHint LayoutHint::operator + ( const LayoutHint & other )
{
	auto result = *this;
	return result += other;
}

LayoutHint & LayoutHint::operator += ( const LayoutHint & other )
{
	for (auto & hint : other.hints_)
	{
		auto it = hints_.find( hint.first );
		if (it != hints_.end())
		{
			it->second = ( it->second + hint.second ) / 2.0f;
		}
		else
		{
			hints_.insert( hint );
		}
	}
	return *this;
}

float LayoutHint::match( const LayoutTags & tags ) const
{
	float total = 0.f;
	float matched = 0.f;
	for (auto & hint : hints_)
	{
		total += hint.second;
		if (std::find_if( tags.tags_.begin(), tags.tags_.end(), 
			[&]( std::string tag )->bool { return stricmp( hint.first.c_str(), tag.c_str() ) == 0; } ) 
			!= tags.tags_.end())
		{
			matched += hint.second;
		}
	}
	if (total > 0.f)
	{
		return matched / total;
	}
	return 0.f;
}