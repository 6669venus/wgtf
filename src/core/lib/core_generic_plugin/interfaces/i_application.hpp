#ifndef I_APPLICATION_HPP
#define I_APPLICATION_HPP

class IApplication
{
public:
	virtual ~IApplication() {}

	virtual int startApplication() = 0;
	virtual void quitApplication() = 0;
};

#endif //I_APPLICATION_HPP

