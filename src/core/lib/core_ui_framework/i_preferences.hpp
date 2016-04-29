#ifndef I_PREFERENCE_HPP
#define I_PREFERENCE_HPP

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/mutable_vector.hpp"

class IPreferencesListener
{
public:
    virtual void preferencesChanged() {}
};

class IPreferences
{
public:
	virtual ~IPreferences() {}

    typedef MutableVector< std::weak_ptr< IPreferencesListener > > PreferencesListeners;

	virtual GenericObjectPtr & getPreference( const char * key ) = 0;
    virtual void registerPreferencesListener(
        std::shared_ptr< IPreferencesListener > & listener ) = 0;
    virtual void deregisterPreferencesListener(
        std::shared_ptr< IPreferencesListener > & listener ) = 0;

    virtual void savePreferenceToFile( const char * filePath ) {}
    virtual void loadPreferenceFromFile( const char * filePath ) {}


};



#endif //I_PREFERENCE_HPP