#ifndef TEST_COMMAND_SYSTEM_FIXTURE_HPP
#define TEST_COMMAND_SYSTEM_FIXTURE_HPP

#include <memory>
#include "core_command_system/i_command_event_listener.hpp"

class ObjectManager;
class CommandManager;
class ISerializationManager;
class ICommandManager;
class Command;
class ReflectionSerializer;
class IDefinitionManager;
class IObjectManager;
class ReflectionController;

class TestCommandSystemFixture
	: public ICommandEventListener
{
public:
	TestCommandSystemFixture();
	~TestCommandSystemFixture();
	IObjectManager & getObjectManager() const;
	IDefinitionManager & getDefinitionManager() const;
	ICommandManager & getCommandSystemProvider() const;
	ReflectionController & getReflectionController() const;
	const ICommandEventListener::MultiCommandStatus & getMultiCommandStatus() const;
	void multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus multiCommandStatus ) const override;

private:
	std::unique_ptr< ObjectManager > objectManager_;
	std::unique_ptr< IDefinitionManager > definitionManager_;
	std::unique_ptr< CommandManager > commandManager_;
	std::unique_ptr< ISerializationManager > serializationManager_;
	std::unique_ptr< Command > setReflectedPropertyCmd_;
	std::unique_ptr< ReflectionSerializer > reflectionSerializer_;
	std::unique_ptr< ReflectionController > reflectionController_;
	mutable ICommandEventListener::MultiCommandStatus multiCommandStatus_;

};
#endif //TEST_COMMAND_SYSTEM_FIXTURE_HPP
