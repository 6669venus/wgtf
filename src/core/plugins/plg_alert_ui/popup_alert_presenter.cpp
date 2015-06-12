#include "alert_models.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "logging_system/interfaces/i_logging_system.hpp"
#include "logging_system/log_level.hpp"
#include "metadata/alert_models.mpp"
#include "popup_alert_presenter.hpp"
#include "reflection/type_class_definition.hpp"
#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"

#include <QObject>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

PopupAlertPresenter::PopupAlertPresenter( IContextManager & contextManager )
	: contextManager_( &contextManager )
	, alertCounter_( 0 )
{		
	// Setup the alert page model
	auto definitionManager = 
		contextManager.queryInterface<IDefinitionManager>();
	assert( definitionManager != nullptr );

	alertPageModel_ = definitionManager->createT< AlertPageModel >();
	assert( alertPageModel_ != nullptr );

	alertPageModel_->init( contextManager );
		
	// Setup the display via QML with the model as input
	auto uiApplication = contextManager.queryInterface< IUIApplication >();
	assert( uiApplication != nullptr );
	
	IUIFramework* qtFramework = contextManager.queryInterface<IUIFramework>();
	assert( qtFramework != nullptr );

	alertWindow_ = qtFramework->createView(
		"qrc:///default/alert_window.qml",
		IUIFramework::ResourceType::Url, alertPageModel_ );
	uiApplication->addView( *alertWindow_ );

	ILoggingSystem* loggingSystem = 
		contextManager.queryInterface< ILoggingSystem >();
	if (loggingSystem != nullptr)
	{
		AlertManager* alertManager = loggingSystem->getAlertManager();
		if (alertManager != nullptr)
		{
			auto uiApplication = contextManager.queryInterface< IUIApplication >();
			if ( nullptr != uiApplication )
			{
				testAddAlert_ = qtFramework->createAction( "AddTestAlert", std::bind( &PopupAlertPresenter::addTestAlert, this ) );
				uiApplication->addAction( *testAddAlert_ );
			}
		}
	}
}

PopupAlertPresenter::~PopupAlertPresenter()
{
	testAddAlert_.reset();
}

void PopupAlertPresenter::show( const char* text )
{
	alertPageModel_->addAlert( text );
}

void PopupAlertPresenter::addTestAlert()
{
	ILoggingSystem* loggingSystem = 
		contextManager_->queryInterface< ILoggingSystem >();
	if (loggingSystem != nullptr)
	{
		AlertManager* alertManager = loggingSystem->getAlertManager();
		if (alertManager != nullptr)
		{
			loggingSystem->log( LOG_ALERT, "This is test alert #%d", alertCounter_++ );
		}
	}
}

