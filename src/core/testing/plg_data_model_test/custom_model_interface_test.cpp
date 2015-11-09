#include "custom_model_interface_test.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"


	class ICustomModelInterface
	{
		DECLARE_REFLECTED

	public:
		ICustomModelInterface()
		{

		}

	protected:
		ICustomModelInterface( int numeric, std::string string )
			: numeric_( numeric )
			, string_( string )
		{

		}

		void incrementNumeric( double value )
		{
			numeric_ += static_cast< int >( value );
		}

		void undoIncrementNumeric( double value )
		{
			numeric_ -= static_cast< int >( value );
		}

	private:
		int numeric_;
		std::string string_;
	};

	class CustomModelImplementation1 : public ICustomModelInterface
	{
	public:
		CustomModelImplementation1()
			: ICustomModelInterface( 1, "Implementation 1" )
		{

		}
	};

	class CustomModelImplementation2 : public ICustomModelInterface
	{
	public:
		CustomModelImplementation2()
			: ICustomModelInterface( 2, "Implementation 2" )
		{

		}
	};

	class CustomModelImplementation3 : public ICustomModelInterface
	{
	public:
		CustomModelImplementation3()
			: ICustomModelInterface( 3, "Implementation 3" )
		{

		}
	};

	class TestFixture
	{
		DECLARE_REFLECTED

	public:
		void init( IDefinitionManager * defManager )
		{
			auto def = defManager->getDefinition< ICustomModelInterface >();
			implementation1_ = std::unique_ptr< ICustomModelInterface >(
				new CustomModelImplementation1 );
			implementation2_ = std::unique_ptr< ICustomModelInterface >(
				new CustomModelImplementation2 );
			implementation3_ = std::unique_ptr< ICustomModelInterface >(
				new CustomModelImplementation3 );
		}

		ICustomModelInterface * implementation1() const
		{
			return implementation1_.get();
		}

		ICustomModelInterface * implementation2() const
		{
			return implementation2_.get();
		}

		ICustomModelInterface * implementation3() const
		{
			return implementation3_.get();
		}

	private:
		std::unique_ptr< ICustomModelInterface > implementation1_;
		std::unique_ptr< ICustomModelInterface > implementation2_;
		std::unique_ptr< ICustomModelInterface > implementation3_;
	};


BEGIN_EXPOSE( ICustomModelInterface, MetaNone() )
	EXPOSE( "numeric", numeric_, MetaNone() )
	EXPOSE( "string", string_, MetaNone() )
	EXPOSE_METHOD( "incrementNumeric", incrementNumeric, undoIncrementNumeric )
END_EXPOSE()

BEGIN_EXPOSE( TestFixture, MetaNone() )
	EXPOSE( "Implementation1", implementation1, MetaNone() )
	EXPOSE( "Implementation2", implementation2, MetaNone() )
	EXPOSE( "Implementation3", implementation3, MetaNone() )
END_EXPOSE()

CustomModelInterfaceTest::CustomModelInterfaceTest()
{

}

CustomModelInterfaceTest::~CustomModelInterfaceTest()
{

}

void CustomModelInterfaceTest::initialise( IComponentContext & contextManager )
{
	auto defManager = contextManager.queryInterface< IDefinitionManager >();
	if (defManager == nullptr)
	{
		return;
	}

	defManager->registerDefinition( new TypeClassDefinition<
		ICustomModelInterface >() );
	defManager->registerDefinition( new TypeClassDefinition<
		TestFixture >() );

	auto testFixture = defManager->create< 
		TestFixture >();
	testFixture->init( defManager );

	auto pQtFramework = contextManager.queryInterface< IQtFramework >();
	if (pQtFramework != nullptr)
	{
		testView_ = pQtFramework->createView( "plg_data_model_test/custom_model_interface_test_panel.qml",
			IUIFramework::ResourceType::Url, testFixture );
	}

	auto uiApplication = contextManager.queryInterface< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->addView( *testView_ );
}

void CustomModelInterfaceTest::fini( IComponentContext & contextManager )
{
	auto uiApplication = contextManager.queryInterface< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->removeView( *testView_ );
}
