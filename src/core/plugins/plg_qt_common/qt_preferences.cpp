#include "qt_preferences.hpp"

//------------------------------------------------------------------------------
QtPreferences::QtPreferences( IDefinitionManager & definitionManger )
	: definitionManager_( definitionManger )
{
}

//------------------------------------------------------------------------------
QtPreferences::~QtPreferences()
{

}

//------------------------------------------------------------------------------
GenericObjectPtr & QtPreferences::getPreference( const char * key )
{
	auto findIt = preferences_.find( key );
	if (findIt != preferences_.end())
	{
		return findIt->second;
	}
	auto preference = GenericObject::create( definitionManager_ );
	preferences_[ key ] = preference;
	return preferences_[ key ];
}

//------------------------------------------------------------------------------
void QtPreferences::savePrferences()
{

}

//------------------------------------------------------------------------------
void QtPreferences::loadPreferences()
{

}