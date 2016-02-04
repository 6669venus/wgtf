#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_variant/variant.hpp"
#include "panel_context.hpp"
#include "python_loader.hpp"
#include "python_panel.hpp"

#include <memory>


struct Python27TestUIPlugin
	: public PluginMain
{
	Python27TestUIPlugin( IComponentContext& componentContext )
	{
	}


	bool PostLoad( IComponentContext& componentContext ) override
	{
		return true;
	}


	void Initialise( IComponentContext& componentContext ) override
	{
		// Initialise variant system; this is required for every plugin that uses Variant.
		auto metaTypeManager = componentContext.queryInterface<IMetaTypeManager>();
		Variant::setMetaTypeManager( metaTypeManager );

		auto pDefinitionManager = componentContext.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
			return;
		}
		auto & definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION( PanelContext );

		ObjectHandle module;
		const auto loaded = PythonLoader::createPythonObjects( componentContext,
			L"..\\..\\..\\src\\core\\testing\\plg_python27_ui_test\\scripts",
			"test_objects",
			module );
		if (!loaded)
		{
			NGT_ERROR_MSG( "Could not load from scripts\n" );
			return;
		}

		pythonPanel1_.reset( new PythonPanel( "Python Test 1",
			componentContext,
			module ) );
		pythonPanel2_.reset( new PythonPanel( "Python Test 2",
			componentContext,
			module ) );
	}


	bool Finalise( IComponentContext& componentContext ) override
	{
		pythonPanel2_.reset();
		pythonPanel1_.reset();
		return true;
	}


	void Unload( IComponentContext& componentContext ) override
	{
	}


	std::unique_ptr< PythonPanel > pythonPanel1_;
	std::unique_ptr< PythonPanel > pythonPanel2_;
};


PLG_CALLBACK_FUNC( Python27TestUIPlugin )
