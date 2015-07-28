#ifndef POPUP_ALERT_PRESENTER_HPP
#define POPUP_ALERT_PRESENTER_HPP

#include "logging_system/alerts/i_alert_presenter.hpp"
#include "reflection/object_handle.hpp"
#include "ui_framework/i_view.hpp"

class AlertPageModel;
class IAction;
class IComponentContext;
class QQuickView;
class QQmlContext;
class QObject;

class PopupAlertPresenter : public IAlertPresenter
{
public:

	explicit PopupAlertPresenter( IComponentContext & contextManager );
	virtual ~PopupAlertPresenter();

	virtual void show( const char* text );

	void addTestAlert();

private:
	
	mutable IComponentContext* contextManager_;
	std::unique_ptr<IView> alertWindow_;
	ObjectHandleT< AlertPageModel > alertPageModel_;

	std::unique_ptr< IAction > testAddAlert_;
	int alertCounter_;
 };

#endif // POPUP_ALERT_PRESENTER_HPP