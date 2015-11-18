#include "wg_context_menu.hpp"
#include "core_dependency_system/di_ref.hpp"
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
#include <QQuickWindow>

struct WGContextMenu::Implementation
{
	Implementation()
	{
	}

	void onComponentComplete( WGContextMenu * contextMenu )
	{
		auto context = QQmlEngine::contextForObject( contextMenu );
		assert( context != nullptr );
		auto componentContextProperty = context->contextProperty( "componentContext" );
		assert( componentContextProperty.isValid() );
		auto componentContextVariant = QtHelpers::toVariant( componentContextProperty );
		assert( !componentContextVariant.isVoid() );
		auto componentContextHandle = componentContextVariant.value< ObjectHandle >();
		assert( componentContextHandle != nullptr );
		auto componentContext = componentContextHandle.getBase< IComponentContext >();
		assert( componentContext != nullptr );
		uiApplication_.reset( new DIRef< IUIApplication >( *componentContext ) );
	}

	void onWindowChanged( QQuickWindow * window )
	{
		assert( uiApplication_ != nullptr );
		auto uiApplication = uiApplication_->get();
		if (uiApplication == nullptr)
		{
			return;
		}

		if (qtContextMenu_ != nullptr)
		{
			uiApplication->removeMenu( *qtContextMenu_ );
		}

		qMenu_.reset( new QMenu() );
		qMenu_->setProperty( "path", QString( path_.c_str() ) );

		qtContextMenu_.reset( new QtContextMenu( *qMenu_ ) );
		uiApplication->addMenu( *qtContextMenu_ );
	}

	std::unique_ptr< DIRef< IUIApplication > > uiApplication_;
	std::unique_ptr< QtContextMenu > qtContextMenu_;
	std::unique_ptr< QMenu > qMenu_;
	std::string path_;
	Variant contextObject_;
	QtConnectionHolder connections_;
};

WGContextMenu::WGContextMenu( QQuickItem * parent )
	: QQuickItem( parent )
	, impl_( new Implementation() )
{
	impl_->connections_ += QObject::connect( 
		this, &QQuickItem::windowChanged, [&]( QQuickWindow * window ) { impl_->onWindowChanged( window ); } );
}

WGContextMenu::~WGContextMenu()
{
	impl_->connections_.reset();
}

void WGContextMenu::componentComplete()
{
	// Derived classes should call the base class method before adding their
	// own actions to perform at componentComplete.
	QQuickItem::componentComplete();
	impl_->onComponentComplete( this );
}

void WGContextMenu::show()
{
	auto contextMenu = impl_->qtContextMenu_.get();
	if (contextMenu == nullptr)
	{
		return;
	}

	// Prepare and display the menu and signal
	prepareMenu( contextMenu );
	contextMenu->getQMenu().popup( QCursor::pos() );
	emit opened();
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

QVariant WGContextMenu::getContextObject() const
{
	return QtHelpers::toQVariant( impl_->contextObject_ );
}

void WGContextMenu::setContextObject( const QVariant& object )
{
	impl_->contextObject_ = QtHelpers::toVariant( object );
}
