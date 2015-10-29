#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "tinyxml2.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <stdio.h>

namespace
{
	enum class ModelPropertyValueType : uint8_t
	{
		String = 0,
		Boolean,
		Integer,
		Double,
		EnumList,
		Vector2,
		Vector3,
		Vector4,
	};
}

//==============================================================================
class DemoTestPlugin
	: public PluginMain
{
private:
	GenericListT<GenericObjectPtr> objList_;
	std::unique_ptr< IView > centralView_;
	std::unique_ptr< IView > propertyView_;


	// TODO:remove tiny xml dependency and use our own serialization stuff to handle this
	bool loadDemoData( IComponentContext & contextManager )
	{
		auto definitionManager = contextManager.queryInterface< IDefinitionManager >();
		if (definitionManager == nullptr)
		{
			return false;
		}
		tinyxml2::XMLDocument doc;
		doc.LoadFile( "sceneModel.xml" );
		if (doc.ErrorID())
		{
			return false;
		}
		auto root = doc.RootElement();
		auto node = root->FirstChildElement( "object" );
		while (node != nullptr)
		{
			auto genericObject = GenericObject::create( *definitionManager );
			objList_.push_back( genericObject );
			populateDemoObject( genericObject, *node );
			node = node->NextSiblingElement( "object" );
		}
		return true;
	}

	void populateDemoObject( GenericObjectPtr & genericObject, const tinyxml2::XMLNode& objectNode )
	{
		auto propertyNode = objectNode.FirstChildElement( "property" );
		while (propertyNode != nullptr)
		{
			uint8_t valueType = 0;
			const char * propertyName = propertyNode->Attribute( "name" );
			auto attribute = propertyNode->FindAttribute( "type" );
			if (attribute != nullptr)
			{
				valueType = static_cast<uint8_t>( attribute->UnsignedValue() );
			}
			std::string tmp("");
			const char * value = propertyNode->GetText();
			if (value != nullptr)
			{
				tmp = value;
			}
			switch( valueType )
			{
			case ModelPropertyValueType::Boolean:
				{
					bool realValue = false;
					tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
					bool isOk = tinyxml2::XMLUtil::ToBool( tmp.c_str(), &realValue );
					assert( isOk );
					genericObject->set( propertyName, realValue );
				}
				break;
			case ModelPropertyValueType::Integer:
				{
					int realValue = -1;
					tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
					bool isOk = tinyxml2::XMLUtil::ToInt( tmp.c_str(), &realValue );
					assert( isOk );
					genericObject->set( propertyName, realValue );
				}
				break;
			case ModelPropertyValueType::Double:
				{
					double realValue = -1.0;
					tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
					bool isOk = tinyxml2::XMLUtil::ToDouble( tmp.c_str(), &realValue );
					assert( isOk );
					genericObject->set( propertyName, realValue );
				}
				break;
			case ModelPropertyValueType::EnumList:
				break;
			case ModelPropertyValueType::Vector2:
				{
					tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
					float d[2] = {0.0f, 0.0f};
					char * pch = nullptr;
					pch = strtok (const_cast<char *>(tmp.c_str()),",");
					int i = 0;
					while (pch != nullptr)
					{
						float v = static_cast<float>( atof( pch ) );
						d[i++] = v;
						pch = strtok (nullptr, ",");
					}
					assert( i == 2 );
					Vector2 realValue( d[0], d[1] );
					genericObject->set( propertyName, realValue );
				}
				break;
			case ModelPropertyValueType::Vector3:
				{
					tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
					float d[3] = {0.0f, 0.0f, 0.0f};
					char * pch = nullptr;
					pch = strtok (const_cast<char *>(tmp.c_str()),",");
					int i = 0;
					while (pch != nullptr)
					{
						float v = static_cast<float>( atof( pch ) );
						d[i++] = v;
						pch = strtok (nullptr, ",");
					}
					assert( i == 3 );
					Vector3 realValue( d[0], d[1], d[2] );
					genericObject->set( propertyName, realValue );
				}
				break;
			case ModelPropertyValueType::Vector4:
				{
					tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
					float d[4] = {0.0f, 0.0f, 0.0f, 0.0f};
					char * pch = nullptr;
					pch = strtok (const_cast<char *>(tmp.c_str()),",");
					int i = 0;
					while (pch != nullptr)
					{
						float v = static_cast<float>( atof( pch ) );
						d[i++] = v;
						pch = strtok (nullptr, ",");
					}
					assert( i == 4 );
					Vector4 realValue( d[0], d[1], d[2], d[3] );
					genericObject->set( propertyName, realValue );
				}
				break;
			default:
				{
					genericObject->set( propertyName, tmp );
				}
				break;
			}
			
			propertyNode = propertyNode->NextSiblingElement( "property" );
		}
	}

public:
	//==========================================================================
	DemoTestPlugin(IComponentContext & contextManager )
	{
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		auto defManager = contextManager.queryInterface< IDefinitionManager >();
		auto controller = contextManager.queryInterface< IReflectionController >();
		if ( uiApplication == nullptr || 
			uiFramework == nullptr || 
			defManager == nullptr ||
			controller == nullptr )
		{
			return ;
		}
		bool success = this->loadDemoData( contextManager );
		if (!success)
		{
			return;
		}
		centralView_ = uiFramework->createView( 
			"plg_demo_test/Framebuffer.qml",
			IUIFramework::ResourceType::Url );

		uiApplication->addView( *centralView_ );

		auto model = std::unique_ptr< ITreeModel >(
			new ReflectedTreeModel( objList_[0], *defManager, controller ) );

		propertyView_ = uiFramework->createView( 
			"plg_demo_test/demo_property_panel.qml", 
			IUIFramework::ResourceType::Url, std::move( model ) );

		uiApplication->addView( *propertyView_ );

	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return false;
		}
		uiApplication->removeView( *propertyView_ );
		uiApplication->removeView( *centralView_ );
		propertyView_ = nullptr;
		centralView_ = nullptr;
		objList_.clear();
		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}

};


PLG_CALLBACK_FUNC(DemoTestPlugin)

