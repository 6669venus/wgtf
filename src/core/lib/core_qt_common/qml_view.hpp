#ifndef QML_VIEW_HPP
#define QML_VIEW_HPP

#include "core_ui_framework/layout_hint.hpp"
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

	bool load( const QUrl & qUrl );

	virtual void focusInEvent() override;
	virtual void focusOutEvent() override;

	virtual void registerListener( IViewEventListener* listener ) override;
	virtual void deregisterListener( IViewEventListener* listener ) override;

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
	typedef std::vector<IViewEventListener*> Listeners;
	Listeners listeners_;
};

#endif//QML_VIEW_HPP
