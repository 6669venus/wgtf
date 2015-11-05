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
class IQtFramework;
class QFileSystemWatcher;

class QmlView : public QObject, public IView
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

	QQuickWidget * releaseView();
	void retainView();
	QQuickWidget * view() const;

	void setContextObject( QObject * object );
	void setContextProperty( const QString & name, const QVariant & property );

	bool load( const QUrl & qUrl );

public slots:
	void error( QQuickWindow::SceneGraphError error, const QString &message );
	void reload();

private:
	bool doLoad(const QUrl & qUrl);

	IQtFramework & qtFramework_;
	std::unique_ptr< QQmlContext > qmlContext_;
	QQuickWidget * quickView_;
	std::string id_;
	std::string title_;
	std::string windowId_;
	LayoutHint hint_;
	QFileSystemWatcher * watcher_;
	bool released_;
	QUrl url_;
};

#endif//QML_VIEW_HPP