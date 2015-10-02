#include "qml_view.hpp"

#include "core_logging/logging.hpp"

#include <cassert>

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QVariantMap>

QmlView::QmlView( QQmlEngine & qmlEngine )
	: qmlContext_( new QQmlContext( qmlEngine.rootContext() ) )
	, quickView_( new QQuickWidget( &qmlEngine, nullptr ) )
	, released_( false )
{
	QQmlEngine::setContextForObject( quickView_, qmlContext_.get() );
}

QmlView::~QmlView()
{
	if (!released_)
	{
		delete quickView_;
	}
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

bool QmlView::load( QUrl & qUrl )
{
	auto qmlEngine = qmlContext_->engine();
	auto qmlComponent = std::unique_ptr< QQmlComponent >(
		new QQmlComponent( qmlEngine, qUrl, quickView_ ) );
	assert( !qmlComponent->isLoading() );
	if (!qmlComponent->isReady())
	{
		NGT_WARNING_MSG( "Error loading control %s\n",
			qPrintable( qmlComponent->errorString() ) );
		return false;
	}

	auto content = std::unique_ptr< QObject >(
		qmlComponent->create( qmlContext_.get() ) );

	auto hintsProperty = content->property( "layoutHints" );
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
	if (windowProperty.isValid())
	{
		windowId_ = windowProperty.toString().toUtf8().data();
	}

	QVariant titleProperty = content->property( "title" );
	if (titleProperty.isValid())
	{
		title_ = titleProperty.toString().toUtf8().data();
	}

	quickView_->setContent( qUrl, qmlComponent.release(), content.release() );
	quickView_->setResizeMode( QQuickWidget::SizeRootObjectToView );
	QObject::connect(
		quickView_, SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)),
    this, SLOT(error(QQuickWindow::SceneGraphError, const QString&)) );
	
	return true;
}
