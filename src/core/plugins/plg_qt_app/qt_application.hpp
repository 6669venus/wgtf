#ifndef QT_APPLICATION_HPP
#define QT_APPLICATION_HPP

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/layout_manager.hpp"

#include "core_common/signal.hpp"

#include <memory>

class IQtFramework;
class QApplication;

class QtApplication : public Implements< IUIApplication >
{
	typedef Signal<void(void)> SignalVoid;

public:
	QtApplication();
	virtual ~QtApplication();

	void initialise( IQtFramework * qtFramework );
	void finalise();
	void update();

	// IApplication
	int startApplication() override;

	// IUIApplication
	void addWindow( IWindow & window ) override;
	void addView( IView & view ) override;
	void addAction( IAction & action ) override;

	const Windows & windows() const override;

	void connectOnUpdate(VoidCallback callback) override;

protected:
	std::unique_ptr< QApplication > application_;

private:
	//void getCommandLine();
	//bool whiteSpace(char c);

	char** argv_;
	int argc_;

	IQtFramework * qtFramework_;
	LayoutManager layoutManager_;
	SignalVoid signalOnUpdate_;
};

#endif//QT_APPLICATION_HPP
