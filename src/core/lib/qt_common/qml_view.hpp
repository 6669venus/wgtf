#ifndef QML_VIEW_HPP
#define QML_VIEW_HPP

#include "ui_framework/i_view.hpp"

#include <memory>
#include <string>

class QObject;
class QUrl;
class QQmlContext;
class QQmlEngine;
class QQuickView;
class QString;
class QVariant;

class QmlView : public IView
{
public:
	QmlView( QQmlEngine & qmlEngine );
	virtual ~QmlView();

	void title( const char * title ) override;
	const char * title() const override;

	void update() override;

	QQuickView * release();
	QQuickView * view() const;

	void setContextObject( QObject * object );
	void setContextProperty( const QString & name, const QVariant & property );

	bool load( QUrl & qUrl );

private:
	std::string title_;
	std::unique_ptr< QQmlContext > qmlContext_;
	QQuickView * quickView_;
	bool released_;
};

#endif//QML_VIEW_HPP