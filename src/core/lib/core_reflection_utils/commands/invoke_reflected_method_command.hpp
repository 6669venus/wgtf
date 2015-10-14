#ifndef INVOKE_REFLECTED_METHOD_COMMAND_HPP
#define INVOKE_REFLECTED_METHOD_COMMAND_HPP

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"

#include <memory>

class IDefinitionManager;

class ReflectedMethodCommandParameters
{
	DECLARE_REFLECTED
public:
	ReflectedMethodCommandParameters();
	ReflectedMethodCommandParameters( const ReflectedMethodCommandParameters& rhs );
	ReflectedMethodCommandParameters& operator=( const ReflectedMethodCommandParameters& rhs );
	virtual ~ReflectedMethodCommandParameters();

	const RefObjectId& getId() const;
	const char* getPath() const;
	const ReflectedMethodParameters& getParameters() const;

	void setId( const RefObjectId& id );
	void setPath( const char* path );
	void setParameters( const ReflectedMethodParameters& parameters );

	static const char* idName();
	static const char* pathName();
	static const char* parametersName();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};


class InvokeReflectedMethodCommand
	: public Command
{
public:
	InvokeReflectedMethodCommand( const IDefinitionManager& definitionManager );
	InvokeReflectedMethodCommand( const InvokeReflectedMethodCommand& rhs );
	InvokeReflectedMethodCommand& operator=( const InvokeReflectedMethodCommand& rhs );
	virtual ~InvokeReflectedMethodCommand();

	const char* getId() const override;
	ObjectHandle execute( const ObjectHandle& arguments ) const override;
	CommandThreadAffinity threadAffinity() const override;
	bool canUndo( const ObjectHandle& arguments ) const override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif //INVOKE_REFLECTED_METHOD_COMMAND_HPP
