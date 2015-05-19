#ifndef _STRING_UTILS_HPP_
#define _STRING_UTILS_HPP_

namespace BW
{
	bool bw_wtoutf8( const wchar_t * src, size_t srcSize, char * dst, size_t dstSize )
	{
		assert(dstSize);
		*dst = '\0';
		return true;
	}
}

#endif // _STRING_UTILS_HPP_