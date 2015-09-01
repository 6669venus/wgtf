#ifndef NGT_APPLICATION_PROXY_HPP
#define NGT_APPLICATION_PROXY_HPP

#include <QtCore/QObject>

#include "core_ui_framework/i_ui_application.hpp"
#include "interfaces/i_application_listener.hpp"
#include "interfaces/i_window_listener.hpp"
#include "interfaces/i_window_adapter.hpp"

class QTimer;
class QWinHost;
class MayaWindow;

class NGTApplicationProxy : public QObject, public IApplicationListener, public IWindowListener
{
	Q_OBJECT
public:
	NGTApplicationProxy( IUIApplication* application, QObject* parent = 0 );

	void applicationStarted() override;
	void applicationStopped() override;
	void windowShow( IWindowAdapter * window ) override;
	void windowHide( IWindowAdapter * window ) override;
	void windowClosed( IWindowAdapter * window ) override;

	bool started() const;
	bool visible() const;

public slots:
	void processEvents();
	void start();
	void stop();
	void show();
	void hide();

private:
	QTimer * timer_;
	IUIApplication* application_;
	std::map< IWindowAdapter*, QWinHost * > windows_;
	MayaWindow * mayaWindow_;
	bool started_;
	bool visible_;
};

#endif//NGT_APPLICATION_PROXY_HPP
