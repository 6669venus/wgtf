#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "logging_system/alerts/alert_manager.hpp"
#include "logging_system/interfaces/i_logging_system.hpp"
#include "logging_system/log_level.hpp"
#include "reflection/i_definition_manager.hpp"
#include "reflection/type_class_definition.hpp"
#include "variant/variant.hpp"
#include "alert_models.hpp"
#include "popup_alert_presenter.hpp"
#include <vector>

/**
 * AlertUIPlugin
 *
 * A plugin used to register the basic alert logger so that it can
 * be used by applications to display alerts. Requires the logging system to
 * also be included in the project. No alerts will be displayed unless an
 * IAlertPresenter is registered with the AlertManager and an ILogger invokes
 * the add() functionality on the AlertManager.
 */

class AlertUIPlugin
	: public PluginMain
{
public:

	AlertUIPlugin( IContextManager & contextManager )
		: popupAlertPresenter_( nullptr )
	{
	}

	bool PostLoad( IContextManager & contextManager ) override
	{		
		auto definitionManager = 
			contextManager.queryInterface<IDefinitionManager>();
		assert( definitionManager != nullptr );

		definitionManager->registerDefinition(
			new TypeClassDefinition<AlertPageModel>() );
		definitionManager->registerDefinition(
			new TypeClassDefinition<AlertObjectModel>() );

		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
 		Variant::setMetaTypeManager( 
 			contextManager.queryInterface< IMetaTypeManager >() );
			
		ILoggingSystem* loggingSystem = 
			contextManager.queryInterface< ILoggingSystem >();
		if (loggingSystem != nullptr)
		{
			AlertManager* alertManager = loggingSystem->getAlertManager();
			if (alertManager != nullptr)
			{
				popupAlertPresenter_ = 
					new PopupAlertPresenter( contextManager );
				alertManager->registerPresenter( popupAlertPresenter_ );

				loggingSystem->enableAlertManagement();
			}
		}
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		ILoggingSystem* loggingSystem = 
			contextManager.queryInterface< ILoggingSystem > ();
		if (loggingSystem != nullptr)
		{
			AlertManager* alertManager = loggingSystem->getAlertManager();
			if (alertManager != nullptr)
			{
				alertManager->unregisterPresenter( popupAlertPresenter_ );
			}

			loggingSystem->disableAlertManagement();
		}

		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
	}

private:

	std::vector< IInterface * > types_;

	PopupAlertPresenter* popupAlertPresenter_;
};

PLG_CALLBACK_FUNC( AlertUIPlugin )
