#include "qml_view.hpp"

#include "core_logging/logging.hpp"
#include "i_qt_framework.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include <cassert>

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QQuickItem>
#include <QVariantMap>
#include <QFileSystemWatcher>

QmlView::QmlView( const char * id, IQtFramework & qtFramework, QQmlEngine & qmlEngine )
	: id_( id )
	, qtFramework_( qtFramework )
	, qmlContext_( new QQmlContext( qmlEngine.rootContext() ) )
	, quickView_( new QQuickWidget( &qmlEngine, nullptr ) )
	, watcher_( nullptr )
	, released_( false )
{
	QQmlEngine::setContextForObject( quickView_, qmlContext_.get() );
	QObject::connect(quickView_, SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)),
		this, SLOT(error(QQuickWindow::SceneGraphError, const QString&)));
}

QmlView::~QmlView()
{
	if (!released_)
	{
		delete quickView_;
	}
}

const char * QmlView::id() const
{
	return id_.c_str();
}

const char * QmlView::title() const
{
	return title_.c_str();
}

const char * QmlView::windowId() const
{
	return windowId_.c_str();
}

const LayoutHint& QmlView::hint() const
{
	return hint_;
}

QQuickWidget * QmlView::releaseView()
{
	released_ = true;
	return view();
}

void QmlView::retainView()
{
	released_ = false;
	quickView_->setParent( nullptr );
}

QQuickWidget * QmlView::view() const
{
	return quickView_;
}

void QmlView::update()
{

}

void QmlView::setContextObject( QObject * object )
{
	qmlContext_->setContextObject( object );
}

void QmlView::setContextProperty(
	const QString & name, const QVariant & property )
{
	qmlContext_->setContextProperty( name, property );
}

void QmlView::error( QQuickWindow::SceneGraphError error, const QString &message )
{
	NGT_ERROR_MSG( "QmlView::error, rendering error: %s\n",
		message.toLatin1().constData() );
}

bool QmlView::load( const QUrl & qUrl )
{
	url_ = qUrl;

	auto preferences = qtFramework_.getPreferences();
	auto preference = preferences->getPreference( id_.c_str() );
	auto value = qtFramework_.toQVariant( preference );
	this->setContextProperty( QString( "Preference" ), value );
	this->setContextProperty( QString( "ViewId" ), id_.c_str() );

	return doLoad( qUrl );
}

bool QmlView::doLoad( const QUrl & url )
{
	auto qmlEngine = qmlContext_->engine();
	qmlEngine->clearComponentCache();

	auto qmlComponent = std::unique_ptr< QQmlComponent >(
		new QQmlComponent( qmlEngine, url, quickView_ ) );
	assert(!qmlComponent->isLoading());
	if (!qmlComponent->isReady())
	{
		NGT_WARNING_MSG( "Error loading control %s\n",
			qPrintable( qmlComponent->errorString() ) );
		return false;
	}

	auto content = std::unique_ptr< QObject >(
		qmlComponent->create(qmlContext_.get()));

	auto hintsProperty = content->property( "layoutHints" );
	hint_.clear();

	if (hintsProperty.isValid())
	{
		auto hintsMap = hintsProperty.value< QVariantMap >();
		for (auto it = hintsMap.cbegin(); it != hintsMap.cend(); ++it)
		{
			hint_ += LayoutHint(
				it.key().toUtf8(), it.value().toFloat() );
		}
	}

	QVariant windowProperty = content->property( "windowId" );
	windowId_ = "";
	if (windowProperty.isValid())
	{
		windowId_ = windowProperty.toString().toUtf8().data();
	}

	QVariant titleProperty = content->property( "title" );
	title_ = "";
	if (titleProperty.isValid())
	{
		title_ = titleProperty.toString().toUtf8().data();
	}

	if (url_.scheme() == "file")
	{
		QVariant autoReload = content->property( "autoReload" );
		if (autoReload.isValid() && autoReload.toBool())
		{
			if (watcher_ == nullptr)
			{
				watcher_ = new QFileSystemWatcher( quickView_ );
			}
			watcher_->addPath( url.toLocalFile() );
			QObject::connect( watcher_, SIGNAL( fileChanged( const QString & ) ), this, SLOT( reload() ) );
		}
		else
		{
			delete watcher_;
			watcher_ = nullptr;
		}
	}

	quickView_->rootObject()->deleteLater();
	quickView_->setContent( url, qmlComponent.release(), content.release() );
	quickView_->setResizeMode( QQuickWidget::SizeRootObjectToView );
	return true;
}

void QmlView::reload()
{
	doLoad( url_ );
}