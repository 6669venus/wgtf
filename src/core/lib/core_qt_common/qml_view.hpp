#ifndef QML_VIEW_HPP
#define QML_VIEW_HPP

#include "core_ui_framework/layout_hint.hpp"
#include "core_common/signal.hpp"

#include "i_qt_view.hpp"
#include <memory>
#include <string>
#include <QObject>
#include <QQuickWindow>

class QObject;
class QUrl;
class QQmlContext;
class QQmlEngine;
class QQuickWidget;
class QString;
class QVariant;
class IQtFramework;
class QFileSystemWatcher;

class QmlView : public QObject, public IQtView
{
	Q_OBJECT
public:
	QmlView( const char * id, IQtFramework & qtFramework, QQmlEngine & qmlEngine );
	virtual ~QmlView();

	const char * id() const override;
	const char * title() const override;
	const char * windowId() const override;
	const LayoutHint& hint() const override;
	void update() override;

	QWidget * releaseView() override;
	void retainView() override;
	QWidget * view() const override;

	void setContextObject( QObject * object );
	void setContextProperty( const QString & name, const QVariant & property );

	bool load(const QUrl & qUrl, const char * customTitle = 0,
		std::function< void() > loadedHandler = [] {}, bool async = true );

	virtual void focusInEvent() override;
	virtual void focusOutEvent() override;

	virtual void registerListener( IViewEventListener* listener ) override;
	virtual void deregisterListener( IViewEventListener* listener ) override;

public slots:
	void error( QQuickWindow::SceneGraphError error, const QString &message );
	void reload();

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};

#endif//QML_VIEW_HPP
