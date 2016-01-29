#ifndef I_PREFERENCE_HPP
#define I_PREFERENCE_HPP

#include "core_reflection/generic/generic_object.hpp"

class IPreferences
{
public:
	virtual ~IPreferences() {}

	virtual GenericObjectPtr & getPreference( const char * key ) = 0;


};

#endif //I_PREFERENCE_HPP