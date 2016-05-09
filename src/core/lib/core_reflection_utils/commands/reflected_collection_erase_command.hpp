#ifndef REFLECTED_COLLECTION_ERASE_COMMAND_HPP
#define REFLECTED_COLLECTION_ERASE_COMMAND_HPP

#include "core_command_system/command.hpp"

class ReflectedCollectionEraseCommand;

class ReflectedCollectionEraseCommandParameters
{
	DECLARE_REFLECTED
public:
	RefObjectId id_;
	std::string path_;
	Variant		key_;

private:
	Variant		value_;

	friend class ReflectedCollectionEraseCommand;
};

class ReflectedCollectionEraseCommand : public Command
{
public:
	ReflectedCollectionEraseCommand( IDefinitionManager & definitionManager );
	virtual ~ReflectedCollectionEraseCommand();

	const char * getId() const override;
	ObjectHandle execute(const ObjectHandle & arguments) const override;
	bool undo( const ObjectHandle & arguments ) const override;
	bool redo( const ObjectHandle & arguments ) const override;
	CommandThreadAffinity threadAffinity() const override;

	ObjectHandle getCommandDescription(const ObjectHandle & arguments) const;

private:
	IDefinitionManager & definitionManager_;
};

#endif // REFLECTED_COLLECTION_ERASE_COMMAND_HPP