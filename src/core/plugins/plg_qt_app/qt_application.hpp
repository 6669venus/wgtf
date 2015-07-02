#ifndef QT_APPLICATION_HPP
#define QT_APPLICATION_HPP

#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/layout_manager.hpp"

#include <memory>

class IQtFramework;
class QApplication;

class QtApplication : public Implements< IUIApplication >
{
public:
	QtApplication();
	virtual ~QtApplication();

	void initialise( IQtFramework * qtFramework );
	void finalise();
	void update();

	// IApplication
	int startApplication() override;
	void processEvents() override;

	// IUIApplication
	void addWindow( IWindow & window ) override;
	void addView( IView & view ) override;
	void addAction( IAction & action ) override;

private:
	IQtFramework * qtFramework_;
	std::unique_ptr< QApplication > application_;
	LayoutManager layoutManager_;
};

#endif//QT_APPLICATION_HPP