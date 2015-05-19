#ifndef _BWRESOURCE_HPP_
#define _BWRESOURCE_HPP_

#include <string>
#include "wg_types/string_ref.hpp"
#include "cstdmf/singleton.hpp"
#include <array>

#define BW_RES_PATH_SEPARATOR ";"

namespace BW
{
class BWResource : public Singleton< BWResource >
{
public:
	static bool init(int, const char*[]) { return true; }
	static bool fini() { return true; }

	static StringRef getFilename( const StringRef& file )
	{
		static std::array<char, 2> sep = {'\\', '/'};
		for (StringRef::size_type i = file.length()-1; i >= 0; --i)
		{
			if (std::find(sep.begin(), sep.end(), file.data()[i]) != sep.end())
				return StringRef(&file.data()[i+1], file.length()-i-1);
		}
		return file;
	}

	static StringRef getExtension( const StringRef& file )
	{
		static std::array<char, 1> sep = {'.'};
		for (StringRef::size_type i = file.length()-1; i >= 0; --i)
		{
			if (std::find(sep.begin(), sep.end(), file.data()[i]) != sep.end())
				return StringRef(&file.data()[i+1], file.length()-i-1);
		}
		return file;
	}

	static StringRef removeExtension( const StringRef& file )
	{
		static std::array<char, 1> sep = {'.'};
		for (StringRef::size_type i = file.length()-1; i >= 0; --i)
		{
			if (std::find(sep.begin(), sep.end(), file.data()[i]) != sep.end())
				return StringRef(file.data(), i);
		}
		return file;
	}

	static bool fileExists( const StringRef& file )
	{
		return false;
	}
};

class BWResolver
{
public:
	static std::string resolveFilename( const StringRef& file )
	{
		return std::string( file.data(), file.length() );
	}
};
}


#endif // _BWRESOURCE_HPP_
