#include "qml_view.hpp"

#include "logging/logging.hpp"

#include <cassert>

#include <QtCore/QVariantMap>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuickWidgets/QQuickWidget>


class QmlViewAdapter : public QObject
{
	Q_OBJECT
public:
	QmlViewAdapter( QmlView& qmlView );
	virtual ~QmlViewAdapter();
	virtual void * nativeWindowId() = 0;
	virtual QWidget * createWidget() = 0;

	QmlView& qmlView() const;

public slots:
	void error( QQuickWindow::SceneGraphError error, const QString &message );

private:
	QmlView& qmlView_;
};

QmlViewAdapter::QmlViewAdapter( QmlView& qmlView )
	: qmlView_( qmlView )
{
}

QmlViewAdapter::~QmlViewAdapter()
{
}

QmlView & QmlViewAdapter::qmlView() const
{
	return qmlView_;
}

void QmlViewAdapter::error( QQuickWindow::SceneGraphError error,
	const QString &message )
{
	NGT_ERROR_MSG( "QmlViewAdapter::error, rendering error: %s\n",
		message.toLatin1().constData() );
}

// QQuickWidget adapter
class QQuickWidgetAdapter : public QmlViewAdapter
{
public:
	QQuickWidgetAdapter( QmlView& qmlView );
	void * nativeWindowId() override;
	QWidget * createWidget() override;
	~QQuickWidgetAdapter();
};

QQuickWidgetAdapter::QQuickWidgetAdapter( QmlView& qmlView )
	: QmlViewAdapter( qmlView )
{
}

void * QQuickWidgetAdapter::nativeWindowId()
{
	NGT_WARNING_MSG( "QQuickWidgetAdapter::nativeWindowId:"
		" QQuickWidget does not support native window ID, "
		"please use QQuickView instead.\n" );
	return nullptr;
}

QQuickWidgetAdapter::~QQuickWidgetAdapter()
{
}


QWidget * QQuickWidgetAdapter::createWidget()
{
	auto component = qmlView().qmlComponent();
	auto context = qmlView().qmlContext();
	auto content = component->create( context );

	auto widget = new QQuickWidget( qmlView().qmlEngine(), nullptr );
	widget->setContent( qmlView().url(), component, content );
	widget->setResizeMode( QQuickWidget::SizeRootObjectToView );
	QObject::connect( widget, &QQuickWidget::sceneGraphError,
		this, &QmlViewAdapter::error );

	return widget;
}


// QQuickView adapter
class QQuickViewAdapter : public QmlViewAdapter
{
public:
	QQuickViewAdapter( QmlView& qmlView );
	void * nativeWindowId() override;
	QWidget * createWidget() override;
	~QQuickViewAdapter();

private:
	QQuickView * createView();
	QQuickView * view_;
};


QQuickViewAdapter::QQuickViewAdapter( QmlView& qmlView )
	: QmlViewAdapter( qmlView )
	, view_( nullptr )
{
	view_ = createView();
}

QQuickView * QQuickViewAdapter::createView()
{
	auto component = qmlView().qmlComponent();
	auto context = qmlView().qmlContext();
	auto content = component->create( context );

	auto view = new QQuickView( qmlView().qmlEngine(), nullptr );
	view->setContent( qmlView().url(), component, content );
	view->setResizeMode( QQuickView::SizeRootObjectToView );
	QObject::connect( view, &QQuickView::sceneGraphError,
		this, &QmlViewAdapter::error );

	return view;
}

void * QQuickViewAdapter::nativeWindowId()
{
	if (!view_)
	{
		view_ = createView();
	}

	return reinterpret_cast< void * >( view_->winId() );
}

QQuickViewAdapter::~QQuickViewAdapter()
{
	if ( view_ && view_->parent() == nullptr )
	{
		view_->deleteLater();
	}
}


QWidget * QQuickViewAdapter::createWidget()
{
	if (!view_)
	{
		view_ = createView();
	}

	auto widget = QWidget::createWindowContainer( view_ );
	widget->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
	widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	widget->setFocusPolicy( Qt::StrongFocus );
	return widget;
}


QmlView::QmlView( QQmlEngine & qmlEngine, const QUrl& url )
	: qmlContext_( new QQmlContext( qmlEngine.rootContext() ) )
	, qmlComponent_( new QQmlComponent( &qmlEngine, url ) )
	, viewAdapter_( nullptr )
{
	if (!qmlComponent_ || !qmlComponent_->isReady())
	{
		NGT_ERROR_MSG( "Error loading control %s\n",
			qPrintable( qmlComponent_->errorString() ) );
	}

	auto content = qmlComponent_->create( qmlContext_.get() );
	content->deleteLater();

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

	auto windowProperty = content->property( "windowId" );
	if (windowProperty.isValid())
	{
		windowId_ = windowProperty.toString().toUtf8();
	}

	auto titleProperty = content->property( "title" );
	if (titleProperty.isValid())
	{
		title_ = titleProperty.toString().toUtf8();
	}
}

QmlView::~QmlView()
{
}


const char * QmlView::title() const
{
	return title_.c_str();
}

const char * QmlView::windowId() const
{
	return windowId_.c_str();
}

void* QmlView::nativeWindowId()
{
	if (viewAdapter_ == nullptr)
	{
		viewAdapter_.reset( new QQuickViewAdapter( *this ) );
	}

	return viewAdapter_->nativeWindowId();
}

const LayoutHint& QmlView::hint() const
{
	return hint_;
}

QWidget * QmlView::createWidget()
{
	if (viewAdapter_ == nullptr)
	{
		viewAdapter_.reset( new QQuickWidgetAdapter( *this ) );
	}

	return viewAdapter_->createWidget();
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


QQmlContext * QmlView::qmlContext() const
{
	return qmlContext_.get();
}

QQmlEngine * QmlView::qmlEngine() const
{
	return qmlContext_->engine();
}

QQmlComponent * QmlView::qmlComponent()
{
	if (qmlComponent_)
	{
		return qmlComponent_.release();
	}

	return new QQmlComponent( qmlEngine(), url() );
}

const QUrl & QmlView::url() const
{
	return url_;
}

#include "qml_view.moc"
