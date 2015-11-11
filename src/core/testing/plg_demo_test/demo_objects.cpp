#include "demo_objects.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include "testing/reflection_objects_test/test_objects.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

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

class DemoObjectsEnvCom : public IEnvComponent
{
	DEFINE_EC_GUID
public:
	DemoObjectsEnvCom() : index_( -1 ) {}

	GenericListT<GenericObjectPtr> objList_;
	int index_;
};

DECLARE_EC_GUID( DemoObjectsEnvCom, 0x157cbf2eu, 0x940a4c9au, 0x97c49525u, 0x180b1f23u );

DemoObjects::DemoObjects()
	: objects_(nullptr)
{
	helper_.value( nullSelection_ );
}

DemoObjects::~DemoObjects()
{
}

bool DemoObjects::init( IComponentContext & contextManager )
{
	auto definitionManager = contextManager.queryInterface< IDefinitionManager >();
	auto fileSystem = contextManager.queryInterface< IFileSystem >();
	auto controller = contextManager.queryInterface< IReflectionController >();
	auto envManager = contextManager.queryInterface< IEnvManager >();
	if ((definitionManager == nullptr) || (controller == nullptr) || 
			(envManager == nullptr) || (fileSystem == nullptr))
	{
		return false;
	}
	pDefManager_ = definitionManager;
	controller_ = controller;
	envManager_ = envManager;
	fileSystem_ = fileSystem;

	envManager_->registerListener( this );
	return true;
}

bool DemoObjects::fini()
{
	envManager_->deregisterListener( this );
	return true;
}

const IValueChangeNotifier * DemoObjects::currentIndexSource() const
{
	return &helper_;
}

void DemoObjects::onAddEnv(IEnvState* state)
{
	ENV_STATE_ADD( DemoObjectsEnvCom, ec );
}

void DemoObjects::onRemoveEnv(IEnvState* state)
{
	ENV_STATE_REMOVE( DemoObjectsEnvCom, ec );
	if (objects_ == ec)
	{
		objects_ = nullptr;
		helper_.value( nullSelection_ );
	}
}

void DemoObjects::onSelectEnv(IEnvState* state)
{
	ENV_STATE_QUERY( DemoObjectsEnvCom, ec );
	if (objects_ != ec)
	{
		objects_ = ec;
		helper_.value( (objects_->index_ >= 0) ? objects_->objList_[objects_->index_] : nullSelection_ );
	}
}

ObjectHandle DemoObjects::getTreeModel() const
{
	auto model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( helper_.value(), *pDefManager_, controller_ ) );
	return std::move( model );
}

const IListModel * DemoObjects::getListModel() const
{
	return &(objects_->objList_);
}

void DemoObjects::updateRootObject( int index )
{
	objects_->index_ = index;
	helper_.value( (objects_->index_ >= 0) ? objects_->objList_[index] : nullSelection_);
}

int DemoObjects::rootObjectIndex()
{
	return objects_->index_;
}

// TODO:remove tiny xml dependency and use our own serialization stuff to handle this
bool DemoObjects::loadDemoData( int idx )
{
	std::string file = "sceneModel";
	file += std::to_string( idx );
	file += ".xml";

	if (!fileSystem_->exists( file.c_str() ))
	{
		return false;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile( file.c_str() ) != tinyxml2::XML_SUCCESS || doc.ErrorID())
	{
		NGT_ERROR_MSG( "Failed to load %s\n", file.c_str() );
		return false;
	}

	auto root = doc.RootElement();
	auto node = root->FirstChildElement( "object" );
	while (node != nullptr)
	{
		auto genericObject = GenericObject::create( *pDefManager_ );
		objects_->objList_.push_back( genericObject );
		populateDemoObject( genericObject, *node );
		node = node->NextSiblingElement( "object" );
	}
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
	return objects_ ? objects_->objList_.size() : 0;
}

Vector3 DemoObjects::getObjectPosition( int index )
{
	assert( objects_ );
	assert( index >= 0 && static_cast<size_t>(index) < objects_->objList_.size() );
	auto genericObject = objects_->objList_[index];
	Vector3 vec3;
	bool isOk = genericObject->get( "position", vec3 );
	assert( isOk );
	return vec3;
}