#include "i_editor_app.hpp"

#include <QApplication>
#include <QWidget>

//==============================================================================
bool IEditorApp::isMinimized()
{
	if (QApplication::activeWindow() == NULL)
	{
		return false;
	}
	return QApplication::activeWindow()->isMinimized();
}

