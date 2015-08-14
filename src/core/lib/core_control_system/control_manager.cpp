#include "control_manager.hpp"

#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"

#include "core_string_utils/string_utils.hpp"
#include <codecvt>

ControlManager::ControlManager()
{
}

ControlManager::~ControlManager()
{
}

void ControlManager::initialise()
{
	uiApplication_ =
		Context::queryInterface< IUIApplication >();
	if (uiApplication_ == nullptr)
	{
		return;
	}

	uiFramework_ =
		Context::queryInterface< IUIFramework >();
	if (uiFramework_ == nullptr)
	{
		return;
	}

	defManager_ = 
		Context::queryInterface< IDefinitionManager >();
	if (defManager_ == NULL)
	{
		return;
	}

	controller_ =
		Context::queryInterface< IReflectionController >();
	if (controller_ == NULL)
	{
		return;
	}

	if (IDefinitionManager * defManager = 
		Context::queryInterface< IDefinitionManager >())
	{
		defManager->getObjectManager()->registerListener(this);
	}

	std::vector< ObjectHandle > objects;
	defManager_->getObjectManager()->getObjects(objects);

	for (const auto& object : objects)
	{
		this->createWidget( object );
	}
}

void ControlManager::finalise()
{

}

void ControlManager::onObjectRegistered( const ObjectHandle & object )
{
	this->createWidget( object );
}

void ControlManager::onObjectDeregistered( const ObjectHandle & object )
{
}

void ControlManager::createWidget( const ObjectHandle & object )
{
	if (objects_.count( object ) != 0)
		return;

	if (object == nullptr)
	{
		return;
	}

	auto metaName =
		findFirstMetaData< MetaDisplayNameObj >( *object.getDefinition() );
	if (metaName != nullptr)
	{
		std::wstring_convert< Utf16to8Facet > conversion( 
			Utf16to8Facet::create() );

		auto treeModel = std::unique_ptr< ITreeModel >(
			new ReflectedTreeModel( object, controller_ ) );
		auto view = uiFramework_->createView( "WGControls/property_tree_view.qml",
			IUIFramework::ResourceType::Url, std::move( treeModel ) );
		uiApplication_->addView( *view );

		objects_[ object ].reset( view.release() );
	}
}
