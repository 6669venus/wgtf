#include "mouse_state.hpp"

#include <QApplication>

/*static */bool MouseState::isMouseDown()
{
	Qt::MouseButtons mouseButtons = QApplication::mouseButtons();
	return	mouseButtons & Qt::LeftButton |
			mouseButtons & Qt::RightButton |
			mouseButtons & Qt::MiddleButton;
}

