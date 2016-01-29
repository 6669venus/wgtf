#ifndef I_QT_VIEW_HPP
#define I_QT_VIEW_HPP

#include "core_ui_framework/i_view.hpp"

class QWidget;

class IQtView : public IView
{
public:
	virtual ~IQtView(){}

	virtual QWidget * releaseView() = 0;
	virtual void retainView() = 0;
	virtual QWidget * view() const = 0;
};

#endif//I_QT_VIEW_HPP
