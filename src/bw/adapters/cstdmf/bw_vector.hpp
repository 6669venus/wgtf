#ifndef BW_VECTOR_HPP
#define BW_VECTOR_HPP

#include <vector>

namespace BW
{
	template <typename T>
	class vector : public std::vector<T>
	{
	};
}

#endif // BW_VECTOR_HPP