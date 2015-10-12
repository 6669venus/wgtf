#ifndef QT_APPLICATION_HPP
#define QT_APPLICATION_HPP

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/layout_manager.hpp"

#include "core_common/signal.hpp"

#include <memory>

class IQtFramework;
class QApplication;
class QSplashScreen;

class QtApplication : public Implements< IUIApplication >
{
	typedef Signal<void(void)> SignalVoid;

public:
	QtApplication( int argc, char** argv );
	virtual ~QtApplication();

	void initialise( IQtFramework * qtFramework );
	void finalise();
	void update();

	// IApplication
	int startApplication() override;
	void quitApplication() override;

	// IUIApplication
	void addWindow( IWindow & window ) override;
	void removeWindow( IWindow & window ) override;
	void addView( IView & view ) override;
	void removeView( IView & view ) override;
	void addAction( IAction & action ) override;
	void removeAction( IAction & action ) override;
	const Windows & windows() const override;

	void connectOnUpdate(VoidCallback callback) override;

protected:
	std::unique_ptr< QApplication > application_;

private:
	int argc_;
	char** argv_;

	IQtFramework * qtFramework_;
	LayoutManager layoutManager_;
	SignalVoid signalOnUpdate_;
	std::unique_ptr< QSplashScreen > splash;
	IWindow* mainWindow_;
};

#endif//QT_APPLICATION_HPP
