#ifndef COMPOUND_COMMAND_HPP
#define COMPOUND_COMMAND_HPP

#include "command.hpp"
#include "core_reflection/reflected_object.hpp"

class MacroObject;

class CompoundCommandArgument
{
	DECLARE_REFLECTED

public:

	const ObjectHandle & getContextObject() const;
	void setContextObject( const ObjectHandle & contextObject );

	static const char * contextObjectPropertyName();

private:
	ObjectHandle	contextObject_;
	std::map<int, ObjectHandle> 

	static const char * s_ContextObjectPropertyName;
};

class CompoundCommand
	: public Command
{
	DECLARE_REFLECTED
public:
	friend CommandManagerImpl;
	typedef std::vector< std::pair<std::string, ObjectHandle> > SubCommandCollection;
	CompoundCommand();
	~CompoundCommand();
	
	const char * getId() const override;
	ObjectHandle execute( const ObjectHandle & arguments ) const override;
	CommandThreadAffinity threadAffinity() const override;
	void addCommand( const char * commandId, const ObjectHandle & commandArguments );
	ObjectHandle getMacroObject() const;
	const SubCommandCollection & getSubCommands() const;

private:
	void initDisplayData( IDefinitionManager & defManager );
	void setId( const char * id );
	
	SubCommandCollection subCommands_;
	std::string id_;
	ObjectHandleT<MacroObject> macroObject_;
};

#endif //COMPOUND_COMMAND_HPP
