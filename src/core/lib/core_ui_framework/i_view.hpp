#ifndef I_VIEW_HPP
#define I_VIEW_HPP

struct LayoutHint;

class IView
{
public:
	virtual ~IView() {}

	virtual const char * title() const = 0;
	virtual const char * windowId() const = 0;
	virtual const LayoutHint& hint() const = 0;
	virtual void update() = 0;
};

#endif // I_VIEW_HPP