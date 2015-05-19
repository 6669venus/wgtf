#ifndef QT_APPLICATION_HPP
#define QT_APPLICATION_HPP

#include "ui_framework/i_ui_application.hpp"

#include <map>
#include <memory>

class IQtFramework;
class QApplication;
class QTimer;

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

	// IUIApplication
	void addWindow( IWindow & window ) override;
	void addFrame( IView & view, LayoutHint & hint, 
		const char * windowId ) override;
	void addPanel( IView & view, LayoutHint & hint, 
		const char * windowId ) override;
	void addAction( IAction & action, const char * path, 
		const char * windowId, const char * shortcut ) override;

private:
	class Layout;
	Layout & getLayout( const char * windowId );

	IQtFramework * qtFramework_;
	std::unique_ptr< QApplication > application_;
	std::map< std::string, std::unique_ptr< Layout > > layouts_;
};

#endif//QT_APPLICATION_HPP