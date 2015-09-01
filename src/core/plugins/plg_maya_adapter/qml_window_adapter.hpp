#ifndef QML_WINDOW_ADAPTER_HPP
#define QML_WINDOW_ADAPTER_HPP

#include "core_qt_common/qml_window.hpp"
#include "i_window_adapter.hpp"
#include "i_window_listener.hpp"

class IQtFramework;
class QQmlEngine;

class QmlWindowAdapter : public QmlWindow, public IWindowAdapter
{
public:
	QmlWindowAdapter( IQtFramework & qtFramework, QQmlEngine & qmlEngine );
	virtual ~QmlWindowAdapter();

	virtual void * nativeWindowId() const override;
	virtual void makeFramelessWindow() override;
	virtual void close() override;
	virtual void show() override;
	virtual void hide() override;
	virtual void addListener( IWindowListener * listner ) override;
	virtual void removeListener( IWindowListener * listener ) override;

private:
	std::vector< IWindowListener * >	listeners_;
};

#endif//QML_WINDOW_ADAPTER_HPP