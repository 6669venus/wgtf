#include "qml_view.hpp"

#include "logging/logging.hpp"

#include <cassert>

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

QmlView::QmlView( QQmlEngine & qmlEngine )
	: qmlContext_( new QQmlContext( qmlEngine.rootContext() ) )
	, quickView_( new QQuickView( &qmlEngine, nullptr ) )
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

void QmlView::title( const char * title )
{
	title_ = title;
}

const char * QmlView::title() const
{
	return title_.c_str();
}

QQuickView * QmlView::release()
{
	released_ = true;
	return view();
}

QQuickView * QmlView::view() const
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
	quickView_->setContent( qUrl, qmlComponent.release(), content.release() );
	quickView_->setResizeMode( QQuickView::SizeRootObjectToView );
	return true;
}