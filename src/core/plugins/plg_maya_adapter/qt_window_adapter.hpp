#ifndef QT_WINDOW_ADAPTER_HPP
#define QT_WINDOW_ADAPTER_HPP

#include "core_qt_common/qt_window.hpp"
#include "i_window_adapter.hpp"
#include "i_window_listener.hpp"

class IQtFramework;
class QIODevice;

class QtWindowAdapter : public QtWindow, public IWindowAdapter
{
public:
	QtWindowAdapter( IQtFramework & qtFramework, QIODevice & source );
	virtual ~QtWindowAdapter();
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