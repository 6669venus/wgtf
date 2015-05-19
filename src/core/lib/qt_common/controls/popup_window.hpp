#ifndef POPUP_WINDOW_HPP
#define POPUP_WINDOW_HPP

#include "../qt_new_handler.hpp"
#include <QQuickWindow>

class PopupWindow
	: public QQuickWindow
{
	Q_OBJECT

	Q_PROPERTY(
		QVariant anchor
		READ getAnchor
		WRITE setAnchor )

	DECLARE_QT_MEMORY_HANDLER

public:
	PopupWindow();
	~PopupWindow();

	QVariant getAnchor();
	void setAnchor( QVariant anchor );

private:
	virtual void mousePressEvent(QMouseEvent * event);
	void onVisibleChanged( bool visible );

	QQuickItem * anchor_;
};


#endif //POPUP_WINDOW_HPP