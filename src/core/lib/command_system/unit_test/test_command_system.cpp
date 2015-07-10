#include "pch.hpp"

#include "unit_test_lib/unit_test.hpp"

#include "test_objects.hpp"
#include "reflection/interfaces/i_base_property.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection_utils/reflection_controller.hpp"
#include "command_system/i_command_manager.hpp"
#include "data_model/generic_list.hpp"
#include "command_system/compound_command.hpp"

TEST_F( TestCommandFixture, runSingleCommand )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		controller.setValue( counter, value );
	}

	{
		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}

}

TEST_F( TestCommandFixture, runBatchCommand )
{
	auto & controller = getReflectionController();

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
		Variant variantValue = controller.getValue( counter );
		CHECK( variantValue.tryCast( value ) );
		CHECK( TEST_VALUE != value );

		std::string text_value;
		Variant variantText = controller.getValue( text );
		CHECK( variantText.tryCast( text_value ) );
		CHECK( TEST_TEXT != text_value );
	}

	{
		int value = TEST_VALUE;
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, value );
		std::string text_value = TEST_TEXT;
		controller.setValue( text, text_value );
		commandSystemProvider.abortBatchCommand();
	}

	{
		int value = 0;
		Variant variantValue = controller.getValue( counter );
		CHECK(variantValue.tryCast( value ));
		CHECK(TEST_VALUE != value);

		std::string text_value;
		Variant variantText = controller.getValue( text );
		CHECK(variantText.tryCast( text_value ));
		CHECK(TEST_TEXT != text_value);
	}

	{
		int value = TEST_VALUE;
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, value );
		std::string text_value = TEST_TEXT;
		controller.setValue( text, text_value );
		commandSystemProvider.endBatchCommand();
	}

	{
		int value = 0;
		Variant variantValue = controller.getValue( counter );
		CHECK(variantValue.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);

		std::string text_value;
		Variant variantText = controller.getValue( text );
		CHECK(variantText.tryCast( text_value ));
		CHECK_EQUAL(TEST_TEXT, text_value);
	}
}

TEST_F( TestCommandFixture, undo_redo )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());
	int oldValue = -1;
	{
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( oldValue ));
	}
	auto & commandSystemProvider = getCommandSystemProvider();
	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		
		controller.setValue( counter, value );
	}

	{
		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}

	{
		commandSystemProvider.undo();
		CHECK(!commandSystemProvider.canUndo());
		CHECK(commandSystemProvider.canRedo());

		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(oldValue, value);
	}

	{
		commandSystemProvider.redo();
		CHECK(commandSystemProvider.canUndo());
		CHECK(!commandSystemProvider.canRedo());

		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}
}


TEST_F( TestCommandFixture, creatMacro )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		controller.setValue( counter, value );
	}

	{
		// TODO: wait on controller
		auto & commandSystemProvider = getCommandSystemProvider();
		auto & history = commandSystemProvider.getHistory();
		commandSystemProvider.createCompoundCommand( history );
		CHECK(commandSystemProvider.getMacros().empty() == false );
	}
}

TEST_F( TestCommandFixture, executeMacro )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		controller.setValue( counter, value );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		while(!commandSystemProvider.canUndo())
		{

		}
		auto & history = commandSystemProvider.getHistory();
		commandSystemProvider.createCompoundCommand( history, "Macro1" );
		CHECK(commandSystemProvider.getMacros().empty() == false );
		auto contextObj = klass_->createManagedObject();
		auto argDef = getDefinitionManager().getDefinition<CompoundCommandArgument>();
		CHECK( argDef != nullptr );
		if (argDef == nullptr)
		{
			return;
		}
		ObjectHandleT<CompoundCommandArgument> arguments = argDef->create();
		arguments->setContextObject( contextObj );
		CommandInstancePtr inst = commandSystemProvider.queueCommand( "Macro1", arguments );
		commandSystemProvider.waitForInstance( inst );
		{
			PropertyAccessor counter = klass_->bindProperty("counter", contextObj );
			int value = 0;
			Variant variant = controller.getValue( counter );
			CHECK(variant.tryCast( value ));
			CHECK_EQUAL(TEST_VALUE, value);
		}
	}
}