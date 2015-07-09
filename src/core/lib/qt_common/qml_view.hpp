#ifndef QML_VIEW_HPP
#define QML_VIEW_HPP

#include "ui_framework/i_view.hpp"
#include "ui_framework/layout_hint.hpp"

#include <memory>
#include <string>
#include <QObject>
#include <QQuickWindow>

class QObject;
class QUrl;
class QQmlContext;
class QQmlEngine;
class QString;
class QVariant;

class QmlViewAdapter;
class QQuickWidgetAdapter;
class QQuickViewAdapter;

class QmlView : public IView
{
public:
	QmlView( QQmlEngine & qmlEngine, const QUrl& url );
	virtual ~QmlView();

	const char * title() const override;
	const char * windowId() const override;
	const LayoutHint& hint() const override;
	void* nativeWindowId() override;

	void update() override;

	QWidget * createWidget();

	void setContextObject( QObject * object );
	void setContextProperty( const QString & name, const QVariant & property );

private:
	friend class QmlViewAdapter;
	friend class QQuickWidgetAdapter;
	friend class QQuickViewAdapter;

	QQmlContext * qmlContext() const;
	QQmlEngine * qmlEngine() const;
	QQmlComponent * qmlComponent();
	const QUrl & url() const;

	std::unique_ptr< QQmlContext > qmlContext_;
	std::string title_;
	std::string windowId_;
	LayoutHint hint_;
	std::unique_ptr< QmlViewAdapter > viewAdapter_;
	QUrl url_;
	std::unique_ptr<QQmlComponent> qmlComponent_;
};

#endif//QML_VIEW_HPP