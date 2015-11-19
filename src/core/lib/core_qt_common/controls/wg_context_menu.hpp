#ifndef WG_CONTEXT_MENU_HPP
#define WG_CONTEXT_MENU_HPP

#include "../qt_new_handler.hpp"
#include <QQuickItem>
#include <QtQml>
#include <memory>

class IListModel;
class QString;
class QtContextMenu;
class QVariant;

class WGContextMenu : public QQuickItem
{
	Q_OBJECT

	// Represents the path of the context menu actions you wish to load. Actions are loaded from
	// XML and include path data. The *.ui file will indicate what your QtContextMenu's path
	// will be and all actions with that path will be added to that menu.
	Q_PROPERTY( QString		path
				READ		getPath
				WRITE		setPath )

	// The object, which may be required for determining if an action should be enabled, checked, or 
	// passed in to the handler function.
	Q_PROPERTY( QVariant	contextObject
				READ		getContextObject
				WRITE		setContextObject )

	DECLARE_QT_MEMORY_HANDLER

protected:

	void componentComplete();

public:

	WGContextMenu( QQuickItem * parent = NULL );
	virtual ~WGContextMenu();

	QString getPath() const;
	void setPath( const QString& path );

	QVariant getContextObject() const;
	void setContextObject( const QVariant& object );

	// QML invokable function to locate the menu based on the provided parameters (windowId and path) and 
	// display it to the end-user.
	Q_INVOKABLE void show();

signals:

	// Signal: onOpened
	// Emitted when show() is complete. May be handled in QML instances of WGContextMenu.
	void opened();

private:

	class Impl;
	std::unique_ptr< Impl > impl_;
};

QML_DECLARE_TYPE( WGContextMenu )

#endif // WG_CONTEXT_MENU_HPP