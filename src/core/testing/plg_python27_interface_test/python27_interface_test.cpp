#include "pch.hpp"
#include "python27_interface_test.hpp"
#include "core_dependency_system/di_ref.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"


IComponentContext * g_contextManager = nullptr;


class TestListener : public PropertyAccessorListener
{
public:
	TestListener();
	void preSetValue( const PropertyAccessor & accessor, const Variant & value ) override;
	void postSetValue( const PropertyAccessor & accessor, const Variant & value ) override;

	bool preSetValueCalled_;
	PropertyAccessor preSetAccessor_;
	Variant preSetValue_;

	bool postSetValueCalled_;
	PropertyAccessor postSetAccessor_;
	Variant postSetValue_;
};


TestListener::TestListener()
	: preSetValueCalled_( false )
	, preSetAccessor_()
	, preSetValue_()
	, postSetValueCalled_( false )
	, postSetAccessor_()
	, postSetValue_()
{
}


void TestListener::preSetValue( const PropertyAccessor & accessor,
	const Variant & value ) /* override */
{
	preSetValueCalled_ = true;
	preSetAccessor_ = accessor;
	preSetValue_ = value;
}


void TestListener::postSetValue( const PropertyAccessor & accessor,
	const Variant & value ) /* override */
{
	postSetValueCalled_ = true;
	postSetAccessor_ = accessor;
	postSetValue_ = value;
}


TEST( Python27Interface )
{
	CHECK( g_contextManager != nullptr );
	if (g_contextManager == nullptr)
	{
		return;
	}
	IComponentContext & contextManager( *g_contextManager );

	DIRef< IPythonScriptingEngine > scriptingEngine( contextManager );
	CHECK( scriptingEngine.get() != nullptr );
	if (scriptingEngine.get() == nullptr)
	{
		return;
	}

	// Import a builtin module
	{
		ObjectHandle module = scriptingEngine->appendPathAndImport( L"", "sys" );
		CHECK( module.isValid() );
	}

	{
		// Import a test module
		const wchar_t* path = L"..\\..\\..\\src\\core\\testing\\plg_python27_interface_test\\scripts";
		const char * moduleName = "python27_test";
		auto module = scriptingEngine->appendPathAndImport( path, moduleName );
		CHECK( module.isValid() );
		if (!module.isValid())
		{
			return;
		}

		DIRef< IDefinitionManager > definitionManager( contextManager );
		CHECK( definitionManager.get() != nullptr );
		if (definitionManager.get() == nullptr)
		{
			return;
		}

		// Test property listeners
		auto testListener = std::make_shared< TestListener >();
		definitionManager->registerPropertyAccessorListener(
			std::static_pointer_cast< PropertyAccessorListener >( testListener ) );

		// Listen to object
		auto moduleDefinition = module.getDefinition( *definitionManager );
		auto testDataAccessor = moduleDefinition->bindProperty( "testData", module );

		// Invoke function
		const auto setResult = testDataAccessor.setValue( 2 );
		const auto success = setResult && !scriptingEngine->checkErrors();

		CHECK( success );

		definitionManager->deregisterPropertyAccessorListener(
			std::static_pointer_cast< PropertyAccessorListener >( testListener ) );
	}

	return;
}

