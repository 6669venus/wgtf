#ifndef BW_MAP_HPP
#define BW_MAP_HPP

#include <map>

namespace BW
{
	template <typename K, typename V>
	class map : public std::map<K, V>
	{
	};
}

#endif // BW_map_HPP