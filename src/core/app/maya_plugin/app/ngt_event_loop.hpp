#ifndef NGT_EVENT_LOOP_HPP
#define NGT_EVENT_LOOP_HPP

#include <QtCore/QObject>

class QTimer;
class IApplication;

class NGTEventLoop : public QObject
{
	Q_OBJECT
public:
	NGTEventLoop( IApplication* application, QObject* parent = 0 );

public slots:
	void processEvents();
	void start();
	void stop();

private:
	QTimer * timer_;
	IApplication* application_;
};

#endif//NGT_EVENT_LOOP_HPP