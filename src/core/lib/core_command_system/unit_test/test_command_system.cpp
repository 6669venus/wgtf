#include "pch.hpp"

#include "core_unit_test/unit_test.hpp"

#include "test_objects.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection_utils/reflection_controller.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_command_system/compound_command.hpp"

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
		controller.getValue( counter );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		auto & history = commandSystemProvider.getHistory();
		commandSystemProvider.createMacro( history );
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
		// TODO: wait on controller
		controller.getValue( counter );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.undo();
		auto & history = commandSystemProvider.getHistory();
		commandSystemProvider.createMacro( history, "Macro1" );
		CHECK(commandSystemProvider.getMacros().empty() == false );
		auto argDef = getDefinitionManager().getDefinition<CompoundCommandArgument>();
		CHECK( argDef != nullptr );
		if (argDef == nullptr)
		{
			return;
		}
		CommandInstancePtr inst = commandSystemProvider.queueCommand( "Macro1", nullptr );
		commandSystemProvider.waitForInstance( inst );
		{
			PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
			int value = 0;
			Variant variant = controller.getValue( counter );
			CHECK(variant.tryCast( value ));
			CHECK_EQUAL(TEST_VALUE, value);
		}
	}
}