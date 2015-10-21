#ifndef WG_CONTEXT_MENU_HPP
#define WG_CONTEXT_MENU_HPP

#include "../qt_new_handler.hpp"
#include <QQuickItem>
#include <QtQml>
#include <memory>

class IListModel;
class QString;
class QVariant;

class WGContextMenu : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY( QString		path
				READ		getPath
				WRITE		setPath
				NOTIFY		pathChanged )
	
	Q_PROPERTY( QString		windowId
				READ		getWindowId
				WRITE		setWindowId
				NOTIFY		windowIdChanged )

	Q_PROPERTY( QVariant	contextObject
				READ		getContextObject
				WRITE		setContextObject
				NOTIFY		contextObjectChanged )

	DECLARE_QT_MEMORY_HANDLER

protected:

	void componentComplete();

public:

	WGContextMenu( QQuickItem * parent = NULL );
	virtual ~WGContextMenu();

	QString getPath() const;
	void setPath( const QString& path );

	QString getWindowId() const;
	void setWindowId( const QString& windowId );

	QVariant getContextObject() const;
	void setContextObject( const QVariant& object );

	Q_INVOKABLE void show();

signals:

	void pathChanged();
	void windowIdChanged();
	void contextObjectChanged();

private:

	struct Implementation;
	std::unique_ptr< Implementation > impl_;
};

QML_DECLARE_TYPE( WGContextMenu )

#endif // WG_CONTEXT_MENU_HPP