#include "wg_context_menu.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include <QString>
#include <QVariant>

struct WGContextMenu::Implementation
{
	Implementation( WGContextMenu & self )
		: self_( self )
	{
		// TODO: Remove test code. Only used to verify data is being pushed to the QML item.
		actions_.push_back( "First" );
		actions_.push_back( "Second" );
		actions_.push_back( "Third" );
	}

	WGContextMenu& self_;
	VariantList actions_;
	QVariant contextObject_;
	QQuickItem* view_;
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

	QQmlEngine * engine = qmlEngine( this );

	// Wrap the actions into an ObjectHandle and pass it in to the context
	QQmlContext * context = new QQmlContext( engine->rootContext(), this );		
	ObjectHandle obj = ObjectHandle( (IListModel*) &impl_->actions_ );
	context->setContextProperty( "actions_", QtHelpers::toQVariant( obj ) );

	QUrl qurl = QUrl( "qrc:///qt_common/wg_context_menu.qml" );
	if (!qurl.isValid())
	{
		NGT_ERROR_MSG( "Invalid QUrl\n" );
		return;
	}

	QQmlComponent component( engine, qurl, this );
	assert( !component.isLoading() );

	if (component.isError() || !component.isReady())
	{
		NGT_ERROR_MSG( "Error loading component: %s\n", qPrintable( component.errorString() ) );
	}
	else
	{
		impl_->view_ = qobject_cast< QQuickItem* >( component.create( context ) );
		if (impl_->view_ == nullptr)
		{
			NGT_ERROR_MSG( "Failed to create component\n" );
		}
		else
		{
			impl_->view_->setParent( this );
			impl_->view_->setParentItem( this );

			// Hide until told otherwise
			impl_->view_->setVisible( false );
			
		}
	}
}

void WGContextMenu::show()
{
	if (impl_->view_)
	{
		impl_->view_->setVisible( true );
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

QVariant WGContextMenu::getContextObject() const
{
	return impl_->contextObject_;
}

void WGContextMenu::setContextObject( const QVariant& object )
{
	impl_->contextObject_ = object;
}
