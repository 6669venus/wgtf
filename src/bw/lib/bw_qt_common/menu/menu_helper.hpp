#ifndef MENU_HELPER_HPP
#define MENU_HELPER_HPP

#include <QObject>

#include "guimanager/i_menu_helper.hpp"

class QAction;
class QMainWindow;

class MenuHelper
	: public QObject
	, public BW::GUI::IMenuHelper
{
	Q_OBJECT

public:
	MenuHelper( QMainWindow * mainWindow );

	void setManager( BW::GUI::Manager * manager ) { guiManager_ = manager; }

	void * getMenu();

	int getMenuItemCount( void * menu );

	void deleteMenu( void * menu, int index );

	void setMenuInfo( void * menu, void * menuInfo );

	void destroyMenu( void * menu );

	BW::GUI::Item * getMenuItemInfo(
		void * menu, unsigned int index, void * typeData );

	void setMenuItemInfo( void * menu, unsigned int index, void * info );

	void insertMenuItem( void * menu, unsigned int index, BW::GUI::ItemPtr info );

	void modifyMenu(
		void * menu,
		unsigned int index,
		BW::GUI::ItemPtr info,
		const wchar_t * string );

	void enableMenuItem( void * menu, unsigned int index, MenuState state );

	void checkMenuItem(
		void * menu, unsigned int index, MenuCheckState state );

	void setSeperator( void * menu, unsigned int index );

	void updateText(
		void * hMenu, unsigned index, const std::wstring & newName );

	void * setSubMenu( void * hMenu, unsigned index );

private:
	QAction * getMenuAction( void * menu, unsigned int index );
	void executeAction();

	QMainWindow * mainWindow_;
	BW::GUI::Manager * guiManager_;
};

#endif //MENU_HELPER_HPP