#include "command_line_parser.hpp"
#include <locale>
#include <codecvt>

//==============================================================================
CommandLineParser::CommandLineParser(int argc, char** argv)
	: argc_( argc )
	, argv_( argv )
	, configPath_( "" )
{
	for (int i = 0; i < argc_ - 1; ++i)
	{
		if (::strncmp( argv_[ i ], "--config", sizeof( "--config" ) ) == 0)
		{
			configPath_ = argv_[ i + 1 ];
			break;
		}
	}

	std::wstring_convert< std::codecvt_utf8< wchar_t > > conv;
	configPathW_ = conv.from_bytes( configPath_ );
}

//==============================================================================
int CommandLineParser::argc() const
{
	return argc_;
}

//==============================================================================
char** CommandLineParser::argv() const
{
	return argv_;
}

//==============================================================================
const char* CommandLineParser::pluginConfigPath() const
{
	return configPath_;
}

//==============================================================================
const wchar_t* CommandLineParser::pluginConfigPathW() const
{

	return configPathW_.c_str();

}