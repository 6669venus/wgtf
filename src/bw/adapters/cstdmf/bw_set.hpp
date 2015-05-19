#ifndef BW_SET_HPP
#define BW_SET_HPP

#include <set>

namespace BW
{
	template <typename T>
	class set : public std::set<T>
	{
	};
}

#endif // BW_SET_HPP