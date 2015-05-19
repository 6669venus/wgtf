#include "qt_application.hpp"

#include "qt_common/i_qt_framework.hpp"
#include "qt_common/qml_view.hpp"
#include "qt_common/qt_palette.hpp"
#include "qt_common/qt_window.hpp"

#include "ui_framework/i_action.hpp"

#include <cassert>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>
#include <QTimer>

namespace
{
	class IdleLoop : public QObject
	{
	public:
		IdleLoop( QtApplication & qtApplication, QObject * parent )
			: QObject( parent )
			, qtApplication_( qtApplication )
		{
			timer_ = new QTimer( this );
			timer_->setInterval( 0 );
			QObject::connect( timer_, &QTimer::timeout, [&]() { 
				qtApplication_.update(); 
			} );
		}

	public slots:
		void start()
		{
			qtApplication_.update();
			timer_->start();
		}

		void stop()
		{
			timer_->stop();
		}

	private:
		QtApplication & qtApplication_;
		QTimer * timer_;
	};
}

// TODO: Not qt specific. Move out of qt_application into layout manager
class QtApplication::Layout
{
public:
	Layout() : window_( nullptr )
	{
	}

	IWindow * window()
	{
		return window_;
	}

	void setWindow( IWindow & window )
	{
		if (window_ != nullptr)
		{
			// TODO: assert, warn or reset existing window
			return;
		}

		window_ = &window;
		for (auto & frame : frames_)
		{
			window_->addFrame( *frame.first, frame.second );
		}
		for (auto & panel : panels_)
		{
			window_->addPanel( *panel.first, panel.second );
		}
		for (auto & action : actions_)
		{
			auto & actionPath = action.second;
			window_->addAction( *action.first, actionPath.first.c_str(), actionPath.second.c_str() );
		}
	}

	void addFrame( IView & view, LayoutHint & hint )
	{
		frames_.push_back( std::make_pair( &view, hint ) );
		if (window_ != nullptr)
		{
			window_->addFrame( view, hint );
		}
	}

	void addPanel( IView & view, LayoutHint & hint )
	{
		panels_.push_back( std::make_pair( &view, hint ) );
		if (window_ != nullptr)
		{
			window_->addPanel( view, hint );
		}
	}

	void addAction( IAction & action, const char * path, const char * shortcut )
	{
		if (path == nullptr)
		{
			path = "";
		}
		if (shortcut == nullptr)
		{
			shortcut = "";
		}
		ActionPath actionPath( path, shortcut );
		actions_.push_back( std::make_pair( &action, actionPath ) );
		if (window_ != nullptr)
		{
			window_->addAction( action, path, shortcut );
		}
	}

private:
	IWindow * window_;
	std::vector< std::pair< IView *, LayoutHint > > frames_;
	std::vector< std::pair< IView *, LayoutHint > > panels_;
	typedef std::pair< std::string, std::string > ActionPath;
	std::vector< std::pair< IAction *, ActionPath > > actions_;
};

QtApplication::QtApplication()
	: application_( new QApplication( __argc, __argv ) )
	, qtFramework_( nullptr )
{
	QApplication::setDesktopSettingsAware( false );
	QApplication::setStyle( QStyleFactory::create( "Fusion" ) );
	QApplication::setFont( QFont( "Noto Sans", 9 ) );

	auto dispatcher = QAbstractEventDispatcher::instance();
	auto idleLoop = new IdleLoop( *this, application_.get() );
	QObject::connect( dispatcher, &QAbstractEventDispatcher::aboutToBlock,
		idleLoop, &IdleLoop::start );
	QObject::connect( dispatcher, &QAbstractEventDispatcher::awake,
		idleLoop, &IdleLoop::stop );
	
}

QtApplication::~QtApplication()
{
}

void QtApplication::initialise( IQtFramework * qtFramework )
{
	qtFramework_ = qtFramework;
	assert( qtFramework_ != nullptr );

	auto palette = qtFramework_->palette();
	if (palette != nullptr)
	{
		application_->setPalette( palette->toQPalette() );
	}
}

void QtApplication::finalise()
{

}

void QtApplication::update()
{
	for (auto & layout : layouts_)
	{
		auto window = layout.second->window();
		if (window != nullptr)
		{
			window->update();
		}
	}
}

int QtApplication::startApplication()
{
	assert( application_ != nullptr );
	return application_->exec();
}

void QtApplication::addWindow( IWindow & window )
{
	auto & layout = getLayout( window.id() );
	layout.setWindow( window );
}

void QtApplication::addFrame( IView & view, LayoutHint & hint, 
							 const char * windowId )
{
	auto & layout = getLayout( windowId );
	layout.addFrame( view, hint );
}

void QtApplication::addPanel( IView & view, LayoutHint & hint, 
							 const char * windowId )
{
	auto & layout = getLayout( windowId );
	layout.addPanel( view, hint );
}

void QtApplication::addAction( IAction & action, const char * path, 
							  const char * windowId, const char * shortcut )
{
	auto & layout = getLayout( windowId );
	layout.addAction( action, path, shortcut );
}

QtApplication::Layout & QtApplication::getLayout( const char * windowId )
{
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto & layout = layouts_[windowId];
	if (layout == nullptr)
	{
		layout.reset( new Layout() );
	}
	return *layout;
}