#include "wg_context_menu.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_menu.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/qt_context_menu.hpp"
#include <QMenu>
#include <QString>
#include <QVariant>

struct WGContextMenu::Implementation
{
	Implementation( WGContextMenu & self )
		: self_( self )
		, contextManager_( nullptr )
		, path_( "" )
		, windowId_( "" )
	{
	}

	WGContextMenu& self_;
	Variant contextObject_;
	IComponentContext* contextManager_;
	std::string path_;
	std::string windowId_;
};

WGContextMenu::WGContextMenu( QQuickItem * parent )
	: QQuickItem( parent )
	, impl_( new Implementation( *this ) )
{
}

WGContextMenu::~WGContextMenu()
{
}

void WGContextMenu::componentComplete()
{
	// Derived classes should call the base class method before adding their
	// own actions to perform at componentComplete.
	QQuickItem::componentComplete();
}

void WGContextMenu::show()
{
	if (impl_->contextManager_ != nullptr)
	{
		auto uiApplication = impl_->contextManager_->queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return;
		}

		const Windows & windows = uiApplication->windows();
		auto findIt = windows.find( impl_->windowId_ );
		if (findIt == windows.end())
		{
			qWarning( "Failed to find window: Could not find window: %s \n", impl_->windowId_.c_str() );
			return;
		}

		const Menus & menus = findIt->second->menus();
		for (auto & menu : menus)
		{
			auto menuPath = menu->path();
			if (strcmp( menuPath, impl_->path_.c_str() ) == 0)
			{
				auto contextMenu = dynamic_cast< QtContextMenu* >( menu.get() );
				if (contextMenu != nullptr)
				{
					// Prepare and display the menu and signal
					prepareMenu( contextMenu );
					contextMenu->getQMenu().popup( QCursor::pos() );
					emit opened();
					break;
				}
			}
		}
	}
}

void WGContextMenu::prepareMenu( QtContextMenu* menu )
{
	if (menu == nullptr)
	{
		return;
	}

	// Attach the current context object to the actions so that they can be retrieved by bound functions
	auto actions = menu->getActions();
	for (auto & action : actions)
	{
		action.first->setData( impl_->contextObject_ );
	}
}

QString WGContextMenu::getPath() const
{
	return impl_->path_.c_str();
}

void WGContextMenu::setPath( const QString& path )
{
	impl_->path_ = path.toUtf8().data();
}

QString WGContextMenu::getWindowId() const
{
	return impl_->windowId_.c_str();
}

void WGContextMenu::setWindowId( const QString& windowId )
{
	impl_->windowId_ = windowId.toUtf8().data();
}

QVariant WGContextMenu::getContextManager() const
{
	return QVariant();
}

void WGContextMenu::setContextManager( const QVariant& value )
{
	Variant variant = QtHelpers::toVariant( value );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto contextManager = provider.getBase< IComponentContext >();
			impl_->contextManager_ = contextManager;
		}
	}
}

QVariant WGContextMenu::getContextObject() const
{
	return QtHelpers::toQVariant( impl_->contextObject_ );
}

void WGContextMenu::setContextObject( const QVariant& object )
{
	impl_->contextObject_ = QtHelpers::toVariant( object );
}
