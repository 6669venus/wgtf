#include "demo_objects.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include "testing/reflection_objects_test/test_objects.hpp"
#include "core_data_model/i_item_role.hpp"

#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <stdio.h>
#include <codecvt>


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

DemoObjects::DemoObjects()
{
}



DemoObjects::~DemoObjects()
{
}

bool DemoObjects::init( IComponentContext & contextManager )
{
	auto definitionManager = contextManager.queryInterface< IDefinitionManager >();
	auto controller = contextManager.queryInterface< IReflectionController >();
	if ((definitionManager == nullptr) || (controller == nullptr))
	{
		return false;
	}
	pDefManager_ = definitionManager;
	controller_ = controller;
	loadDemoData( *pDefManager_ );
	return true;
}

const IValueChangeNotifier * DemoObjects::currentIndexSource() const
{
	return &helper_;
}

ObjectHandle DemoObjects::getTreeModel() const
{
	auto model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( helper_.value(), *pDefManager_, controller_ ) );
	return std::move( model );
}

void DemoObjects::updateRootObject( int index )
{
	if ((index == -1))
	{
		return;
	}
	assert(index < static_cast<int>(objList_.size()));
	helper_.value( objList_[index]);
}

// TODO:remove tiny xml dependency and use our own serialization stuff to handle this
bool DemoObjects::loadDemoData( IDefinitionManager & definitionManager )
{
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
		auto genericObject = GenericObject::create( definitionManager );
		objList_.push_back( genericObject );
		populateDemoObject( genericObject, *node );
		node = node->NextSiblingElement( "object" );
	}
	helper_.init( objList_[0] );
	return true;
}

void DemoObjects::populateDemoObject( GenericObjectPtr & genericObject, const tinyxml2::XMLNode& objectNode )
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

size_t DemoObjects::getObjectCount()
{
	return objList_.size();
}

Vector3 DemoObjects::getObjectPosition( int index )
{
	auto genericObject = objList_[index];
	Vector3 vec3;
	bool isOk = genericObject->get( "position", vec3 );
	assert( isOk );
	return vec3;
}