#ifndef I_ALERT_PRESENTER_HPP
#define I_ALERT_PRESENTER_HPP

#include <stdint.h>

class IAlertPresenter
{
public:

	virtual ~IAlertPresenter() {}
	virtual void show( const char* text ) = 0;
};

#endif // I_ALERT_PRESENTER_HPP