#ifndef QT_MENU_HPP
#define QT_MENU_HPP

#include "qt_connection_holder.hpp"
#include "core_ui_framework/i_menu.hpp"

#include <string>
#include <memory>

#include <QAction>

class QMenu;

typedef std::map< IAction *, QAction * > Actions;
typedef std::map< IAction *, std::unique_ptr<QAction> > SharedActions;

class QtMenu : public IMenu
{
public:
	QtMenu( QObject & menu, const char * windowId );

	const char * path() const override;
	const char * windowId() const override;

	void update() override;
	
	const char * relativePath( const char * path ) const;

	QAction * createQAction( IAction & action );
	void destroyQAction( IAction & action );
	QAction * getQAction( IAction & action );

	const Actions& getActions() const;

protected:
	static void addMenuAction( QMenu & qMenu, QAction & qAction, const char * path );
	void removeQAction( QWidget* qWidget, IAction & action, QAction* qAction, const char * path );

private:
	QAction * createSharedQAction( IAction & action );
	QAction * getSharedQAction( IAction & action );

	SharedActions sharedQActions_;

	QObject & menu_;
	Actions actions_;
	
	std::string path_;
	std::string windowId_;
	QtConnectionHolder connections_;
};

#endif//QT_MENU_BAR_HPP