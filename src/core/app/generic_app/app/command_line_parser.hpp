#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_dependency_system/i_interface.hpp"

class CommandLineParser
	: public Implements< ICommandLineParser >
{
public:
	CommandLineParser(int argc, char** argv);
	virtual int argc() const override;
	virtual char** argv() const override;
	virtual const char* pluginConfigPath() const override;
	virtual const wchar_t* pluginConfigPathW() const override;
private:
	int argc_;
	char** argv_;
	std::wstring configPathW_;
	const char* configPath_;
};

#endif // COMMAND_LINE_PARSER_HPP



