#ifndef I_VIEW_HPP
#define I_VIEW_HPP

class IView
{
public:
	virtual ~IView() {}

	virtual void title( const char * title ) = 0;
	virtual const char * title() const = 0;

	virtual void update() = 0;
};

#endif I_VIEW_HPP