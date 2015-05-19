#include "message_box.hpp"

#include <QApplication>
#include <QMessageBox>


int CreateMessageBox( void * parent, const wchar_t * text, const wchar_t * caption,
			   MessageBoxFlags flags )
{
	QWidget * parentWnd =
		parent == NULL ? QApplication::activeWindow() : ( QWidget * ) parent;

	Qt::WindowFlags windowFlags = 
		(	Qt::Dialog |
			Qt::WindowTitleHint |
			Qt::MSWindowsFixedSizeDialogHint |
			Qt::WindowStaysOnTopHint );
	windowFlags &= ~Qt::WindowContextHelpButtonHint;

	QMessageBox::Icon icon = 
		( ( flags & BW_MB_ICONWARNING | flags & BW_MB_ICONEXCLAMATION ) ?
			QMessageBox::Warning :
			QMessageBox::NoIcon );

	QMessageBox msgBox(
		QString::fromWCharArray( caption ),
		QString::fromWCharArray( text ),
		icon, 0,0,0, parentWnd, windowFlags );

	msgBox.setWindowModality( Qt::ApplicationModal );

	QMessageBox::StandardButtons buttons =
		( flags & BW_MB_OK ? QMessageBox::Ok : 0 );
	msgBox.setStandardButtons( buttons );
	return msgBox.exec();
}

