#ifndef NGT_APPLICATION_PROXY_HPP
#define NGT_APPLICATION_PROXY_HPP

#include <QtCore/QObject>

#include "ui_framework/i_ui_application.hpp"

class QTimer;
class QWinHost;
class MayaWindow;

class NGTApplicationProxy : public QObject, IApplicationListener
{
	Q_OBJECT
public:
	NGTApplicationProxy( IUIApplication* application, QObject* parent = 0 );

	void applicationStarted() override;
	void applicationStopped() override;

public slots:
	void processEvents();
	void start();
	void stop();
	void show();
	void hide();

private:
	QTimer * timer_;
	IUIApplication* application_;
	std::vector< QWinHost * > windows_;
	MayaWindow * mayaWindow_;

};

#endif//NGT_APPLICATION_PROXY_HPP
