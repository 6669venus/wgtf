#include "wait_cursor.hpp"
#include <QApplication>

//==============================================================================
WaitCursor::WaitCursor()
{
	QApplication::setOverrideCursor( Qt::WaitCursor );
}

//==============================================================================
WaitCursor::~WaitCursor()
{
	QApplication::restoreOverrideCursor();
}