#ifndef I_VALUE_CHANGE_NOTIFIER_HPP
#define I_VALUE_CHANGE_NOTIFIER_HPP


#include "wg_types/event.hpp"


class Variant;


/**
 *	Interface for accessing ValueChangeNotifier without a templated type.
 */
class IValueChangeNotifier
{
public:
	virtual ~IValueChangeNotifier()
	{
		notifyDestructing();
	}
	virtual Variant variantValue() const = 0;
	virtual bool variantValue( const Variant& data ) = 0;

	PUBLIC_EVENT( IValueChangeNotifier, PreDataChanged )
	PUBLIC_EVENT( IValueChangeNotifier, PostDataChanged )
	PUBLIC_EVENT( IValueChangeNotifier, Destructing )
};


#endif // I_VALUE_CHANGE_NOTIFIER_HPP

