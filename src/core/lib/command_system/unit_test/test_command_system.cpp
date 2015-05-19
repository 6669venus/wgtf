#include "pch.hpp"

#include "unit_test_lib/unit_test.hpp"

#include "test_objects.hpp"
#include "reflection/interfaces/i_base_property.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/interfaces/i_reflection_property_setter.hpp"
#include "command_system/command_system_provider.hpp"
#include "data_model/generic_list.hpp"
#include "command_system/compound_command.hpp"

TEST_F( TestCommandFixture, runSingleCommand )
{
	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		auto & propertySetter = getReflectionPropertySetter();
		propertySetter.setDataValue( counter, value );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		auto & history = commandSystemProvider.getHistory();
		while(history.empty())
		{

		}
		int value = 0;
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}

}

TEST_F( TestCommandFixture, runBatchCommand )
{
	auto objHandle = klass_->createManagedObject();

	const int TEST_VALUE = 57;
	const char * TEST_TEXT = "HelloCommand";

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());
	PropertyAccessor text = klass_->bindProperty( "text", objHandle );
	CHECK(text.isValid());

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		int value = 0;
		Variant variantValue = counter.getValue();
		CHECK( variantValue.tryCast( value ) );
		CHECK( TEST_VALUE != value );

		std::string text_value;
		Variant variantText = text.getValue();
		CHECK( variantText.tryCast( text_value ) );
		CHECK( TEST_TEXT != text_value );
	}

	{
		int value = TEST_VALUE;
		auto & propertySetter = getReflectionPropertySetter();
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		propertySetter.setDataValue( counter, value );
		std::string text_value = TEST_TEXT;
		propertySetter.setDataValue( text, text_value );
		commandSystemProvider.abortBatchCommand();
	}

	{
		auto status = getMultiCommandStatus();
		while(status != ICommandEventListener::MultiCommandStatus_Cancel)
		{
			status = getMultiCommandStatus();
		}
		int value = 0;
		Variant variantValue = counter.getValue();
		CHECK(variantValue.tryCast( value ));
		CHECK(TEST_VALUE != value);

		std::string text_value;
		Variant variantText = text.getValue();
		CHECK(variantText.tryCast( text_value ));
		CHECK(TEST_TEXT != text_value);
	}

	{
		int value = TEST_VALUE;
		auto & propertySetter = getReflectionPropertySetter();
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		propertySetter.setDataValue( counter, value );
		std::string text_value = TEST_TEXT;
		propertySetter.setDataValue( text, text_value );
		commandSystemProvider.endBatchCommand();
	}

	{
		auto status = getMultiCommandStatus();
		while(status != ICommandEventListener::MultiCommandStatus_Complete)
		{
			status = getMultiCommandStatus();
		}
		int value = 0;
		Variant variantValue = counter.getValue();
		CHECK(variantValue.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);

		std::string text_value;
		Variant variantText = text.getValue();
		CHECK(variantText.tryCast( text_value ));
		CHECK_EQUAL(TEST_TEXT, text_value);
	}
}

TEST_F( TestCommandFixture, undo_redo )
{
	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());
	int oldValue = -1;
	{
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( oldValue ));
	}
	auto & propertySetter = getReflectionPropertySetter();
	auto & commandSystemProvider = getCommandSystemProvider();
	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		
		propertySetter.setDataValue( counter, value );
	}

	{
		auto & history = commandSystemProvider.getHistory();
		while(history.empty())
		{

		}
		int value = 0;
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}

	{
		commandSystemProvider.undo();
		for (int i = 0; i< 10 && commandSystemProvider.canUndo(); ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		CHECK(!commandSystemProvider.canUndo());

		int value = 0;
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(oldValue, value);
	}

	{
		commandSystemProvider.redo();
		while(commandSystemProvider.canRedo())
		{
		}
		int value = 0;
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}
}


TEST_F( TestCommandFixture, creatMacro )
{
	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		auto & propertySetter = getReflectionPropertySetter();
		propertySetter.setDataValue( counter, value );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		auto & history = commandSystemProvider.getHistory();
		while(history.empty())
		{

		}
		commandSystemProvider.createCompoundCommand( const_cast<GenericList &>( history ) );
		CHECK(commandSystemProvider.getMacros().empty() == false );
	}
}

TEST_F( TestCommandFixture, executeMacro )
{
	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		auto & propertySetter = getReflectionPropertySetter();
		propertySetter.setDataValue( counter, value );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		auto & history = commandSystemProvider.getHistory();
		while(history.empty())
		{

		}
		commandSystemProvider.createCompoundCommand( const_cast<GenericList &>( history ), "Macro1" );
		CHECK(commandSystemProvider.getMacros().empty() == false );
		auto contextObj = klass_->createManagedObject();
		ObjectHandleT<CompoundCommandArgument> arguments 
			= commandSystemProvider.createCommandArgumentT<CompoundCommandArgument>();
		arguments->setContextObject( contextObj );
		CommandInstancePtr inst = commandSystemProvider.queueCommand( "Macro1", arguments );
		inst->waitForCompletion();
		{
			PropertyAccessor counter = klass_->bindProperty("counter", contextObj );
			int value = 0;
			Variant variant = counter.getValue();
			CHECK(variant.tryCast( value ));
			CHECK_EQUAL(TEST_VALUE, value);
		}
	}
}