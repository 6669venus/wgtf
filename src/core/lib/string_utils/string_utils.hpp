#ifndef TOOLS_STRING_UTILS_HPP
#define TOOLS_STRING_UTILS_HPP

#include <codecvt>

class Utf16to8Facet
	: public std::codecvt_utf8< wchar_t >
{
private:
	Utf16to8Facet() { }
public:
	static Utf16to8Facet * create()
	{
#if ( ( _MSC_VER == 1700 ) && _DEBUG )
		//Lame workaround because the VC2012 compiler assumes that all 
		//allocations come from the CRT
		//https://connect.microsoft.com/VisualStudio/feedback/details/750951/std-locale-implementation-in-crt-assumes-all-facets-to-be-allocated-on-crt-heap-and-crashes-in-destructor-in-debug-mode-if-a-facet-was-allocated-by-a-custom-allocator
		return _NEW_CRT( Utf16to8Facet );
#else
		return new Utf16to8Facet();
#endif
	}
};

#endif //TOOLS_STRING_UTILS_HPP