#ifndef I_ACTION_HPP
#define I_ACTION_HPP

class IAction
{
public:
	virtual ~IAction() {}

	virtual const char * text() const = 0;
	virtual const char * icon() const = 0;
	virtual const char * windowId() const = 0;
	virtual const char * path() const = 0;
	virtual const char * shortcut() const = 0;

	virtual bool enabled() const = 0;
	virtual bool checked() const = 0;
	virtual bool isCheckable() const = 0;
	virtual void execute() = 0;
};

#endif//I_ACTION_HPP