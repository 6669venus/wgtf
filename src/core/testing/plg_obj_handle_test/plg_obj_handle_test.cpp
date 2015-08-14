#include "core_generic_plugin/generic_plugin.hpp"

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/metadata/meta_types.hpp"

#include "core_data_model/reflection/reflected_list.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"

#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"

class Test1
{
	DECLARE_REFLECTED
public:
	Test1() : value_(0) {}
	Test1(int v) : value_(v) {}

private:
	int value_;
};

BEGIN_EXPOSE( Test1, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
END_EXPOSE()

class Test2
{
	DECLARE_REFLECTED
public:
	Test2() : value_(0) {}
	Test2(int v) : value_(v), test1_(v) {}

private:
	int value_;
	Test1 test1_;
};

BEGIN_EXPOSE( Test2, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test1", test1_, MetaNone() )
END_EXPOSE()

class Test3
{
	DECLARE_REFLECTED
public:
	Test3() : value_(0) {}
	Test3(int v) : value_(v), test2_(v), vector_(v, 1) {}

private:
	int value_;
	Test2 test2_;
	std::vector<Test1> vector_;
};

BEGIN_EXPOSE( Test3, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test2", test2_, MetaNone() )
	EXPOSE( "TestVec", vector_, MetaNone() )
END_EXPOSE()

class Test1Stack
{
	DECLARE_REFLECTED
public:
	Test1Stack() : value_(0) {}
	Test1Stack(int v) : value_(v) {}

private:
	int value_;
};

BEGIN_EXPOSE( Test1Stack, MetaOnStack() )
	EXPOSE( "Value", value_, MetaNone() )
END_EXPOSE()

class Test2Stack
{
	DECLARE_REFLECTED
public:
	Test2Stack() : value_(0) {}
	Test2Stack(int v) : value_(v), test1_(v) {}

private:
	int value_;
	Test1 test1_;
};

BEGIN_EXPOSE( Test2Stack, MetaOnStack() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test1", test1_, MetaNone() )
END_EXPOSE()

class GListTest
{
public:
	GListTest( IDefinitionManager* defManager ) : gl_(defManager) {}
	GListTest(const GListTest& ) : gl_(nullptr) { assert(false); }

	template <typename T>
	void addItem( T& t ) { gl_.emplace_back( t ); }

	ObjectHandle getList() const { return ObjectHandle( &gl_ ); }

private:
	ReflectedList gl_;
};

class TestObjHandlePlugin
	: public PluginMain
{
public:
	TestObjHandlePlugin( IComponentContext & contextManager ) {}

	bool PostLoad( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		if (IDefinitionManager* dm = contextManager.queryInterface<IDefinitionManager>())
		{
			def1_ = dm->registerDefinition( new TypeClassDefinition< Test1 > );
			def2_ = dm->registerDefinition( new TypeClassDefinition< Test2 > );
			def3_ = dm->registerDefinition( new TypeClassDefinition< Test3 > );

			dm->registerDefinition( new TypeClassDefinition< Test1Stack > );
			dm->registerDefinition( new TypeClassDefinition< Test2Stack > );
		}
		return true;
	}

	void Initialise( IComponentContext & contextManager )
	{
		glist_ = std::unique_ptr<GListTest>( new GListTest( contextManager.queryInterface<IDefinitionManager>() ) );
		glist_->addItem( Test1Stack( 5 ) );
		glist_->addItem( Test2Stack( 58 ) );
		glist_->addItem( Test1Stack( 7 ) );

		if (IUIFramework* ui = contextManager.queryInterface<IUIFramework>())
		{
			viewGL_ = ui->createView( "testing/test_list_panel.qml",
				IUIFramework::ResourceType::Url, glist_->getList() );

			test_ = std::unique_ptr<Test3>( new Test3(3) );
			auto model = std::unique_ptr< ITreeModel >( new ReflectedTreeModel(
				ObjectHandle(*test_, def3_), 
				contextManager.queryInterface<IReflectionController>() ) );

			viewTest_ = ui->createView( "testing/test_tree_panel.qml",
				IUIFramework::ResourceType::Url, ObjectHandle(std::move( model )) );

			if (IUIApplication* app = contextManager.queryInterface<IUIApplication>())
			{
				app->addView( *viewGL_ );
				app->addView( *viewTest_ );
			}
		}
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		viewGL_.reset();
		viewTest_.reset();
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
	}

private:
	IClassDefinition* def1_;
	IClassDefinition* def2_;
	IClassDefinition* def3_;

	std::unique_ptr<GListTest> glist_;
	std::unique_ptr<IView> viewGL_;
	std::unique_ptr<Test3> test_;
	std::unique_ptr<IView> viewTest_;
};

PLG_CALLBACK_FUNC( TestObjHandlePlugin )

