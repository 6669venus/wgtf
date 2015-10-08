#ifndef I_COMMAND_LINE_PARSER_HPP
#define I_COMMAND_LINE_PARSER_HPP

class IInterface;

class ICommandLineParser
{
public:
	virtual ~ICommandLineParser() {}
	virtual int argc() const = 0;
	virtual char** argv() const = 0;
	virtual const char* pluginConfigPath() const = 0;
	virtual const wchar_t* pluginConfigPathW() const = 0;
};

#endif // I_COMMAND_LINE_PARSER_HPP
