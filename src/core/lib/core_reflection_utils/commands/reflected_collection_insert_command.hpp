#ifndef REFLECTED_COLLECTION_INSERT_COMMAND_HPP
#define REFLECTED_COLLECTION_INSERT_COMMAND_HPP

#include "core_command_system/command.hpp"

class ReflectedCollectionInsertCommandParameters
{
	DECLARE_REFLECTED
public:
	RefObjectId id_;
	std::string path_;
	Variant		key_;
	Variant		value_;
};

class ReflectedCollectionInsertCommand : public Command
{
public:
	ReflectedCollectionInsertCommand( IDefinitionManager & definitionManager );
	virtual ~ReflectedCollectionInsertCommand();

	const char * getId() const override;
	ObjectHandle execute(const ObjectHandle & arguments) const override;
	bool undo( const ObjectHandle & arguments ) const override;
	bool redo( const ObjectHandle & arguments ) const override;
	CommandThreadAffinity threadAffinity() const override;

	ObjectHandle getCommandDescription(const ObjectHandle & arguments) const;

private:
	IDefinitionManager & definitionManager_;
};

#endif // REFLECTED_COLLECTION_INSERT_COMMAND_HPP