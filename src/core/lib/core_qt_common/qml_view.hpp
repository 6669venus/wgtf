#ifndef QML_VIEW_HPP
#define QML_VIEW_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/layout_hint.hpp"

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

class QmlView : public QObject, public IView
{
	Q_OBJECT
public:
	QmlView( QQmlEngine & qmlEngine );
	virtual ~QmlView();

	const char * title() const override;
	const char * windowId() const override;
	const LayoutHint& hint() const override;
	void update() override;

	QQuickWidget * release();
	QQuickWidget * view() const;

	void setContextObject( QObject * object );
	void setContextProperty( const QString & name, const QVariant & property );

	bool load( QUrl & qUrl );

public slots:
	void error( QQuickWindow::SceneGraphError error, const QString &message );

private:
	std::unique_ptr< QQmlContext > qmlContext_;
	QQuickWidget * quickView_;

	std::string title_;
	std::string windowId_;
	LayoutHint hint_;

	bool released_;
};

#endif//QML_VIEW_HPP