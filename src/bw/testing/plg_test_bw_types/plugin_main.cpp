#include "generic_plugin/generic_plugin.hpp"

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"

#include "reflection/reflected_object.hpp"

#include "reflection/reflection_macros.hpp"
#include "reflection/function_property.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"
#include "reflection/metadata/meta_types.hpp"

#include "data_model/reflection/reflected_tree_model.hpp"

#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_view.hpp"
#include "ui_framework/i_ui_application.hpp"

class TestVectors
{
	DECLARE_REFLECTED
public:
	TestVectors()
		: v3_(40.6f, 49.2f, 0.3f)
		, v4_(39.7f, 95.9f, 32.5f, 74.8f)
		, c3_(14.f, 39.f, 180.f)
		, c4_(236.f, 229.f, 13.f, 255.0f)
	{
	}

private:
	void setVector4( const BW::Vector4 & vec4 ) { v4_ = vec4; }
	void getVector4( BW::Vector4 * vec4 ) const { *vec4 = v4_; }

	void setColor3( const BW::Vector3 & color ) { c3_ = color; }
	BW::Vector3 getColor3() const { return c3_; }

	BW::Vector3 v3_;
	BW::Vector4 v4_;
	BW::Vector3 c3_;
	BW::Vector4 c4_;
};

BEGIN_EXPOSE( TestVectors, MetaNone() )
	EXPOSE( "Vector3", v3_, MetaNone() )
	EXPOSE( "Vector4", getVector4, setVector4, MetaNone() )
	EXPOSE( "Color3", getColor3, setColor3, MetaColor() )
	EXPOSE( "Color4", c4_, MetaColor() )
END_EXPOSE()


class TestBWTypesPlugin
	: public PluginMain
{
public:
	TestBWTypesPlugin( IComponentContext & contextManager ) {}

	bool PostLoad( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		if (IDefinitionManager* dm = contextManager.queryInterface<IDefinitionManager>())
		{
			initReflectedTypes( *dm );
			data_ = dm->create<TestVectors>();
		}
		return true;
	}

	void Initialise( IComponentContext & contextManager )
	{
		auto model = std::unique_ptr< ITreeModel >( new ReflectedTreeModel( data_,
				contextManager.queryInterface<IReflectionController>() ) );

		if (IUIFramework* ui = contextManager.queryInterface<IUIFramework>())
		{
			view_ = ui->createView( "qrc:///testing/test_tree_panel.qml",
				IUIFramework::ResourceType::Url, std::move( model ) );

			if (IUIApplication* app = contextManager.queryInterface<IUIApplication>())
			{
				app->addView( *view_ );
			}
		}
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		view_.reset();
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{

	}

private:
	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
		REGISTER_DEFINITION(TestVectors);
	}

	ObjectHandle data_;
	std::unique_ptr<IView> view_;
};

PLG_CALLBACK_FUNC( TestBWTypesPlugin )

