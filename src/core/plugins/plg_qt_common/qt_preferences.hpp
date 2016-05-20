#ifndef QT_PREFERENCES_HPP
#define QT_PREFERENCES_HPP

#include "core_ui_framework/i_preferences.hpp"

namespace wgt
{
class IComponentContext;
namespace
{
    class QtPreferencesImpl;
}

class QtPreferences : public Implements< IPreferences >
{
public:
	QtPreferences();
	~QtPreferences();
    void init( IComponentContext& contextManager );
    void fini();
	GenericObjectPtr & getPreference( const char * key ) override;
    void registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener ) override;
    void deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener ) override;
    void writePreferenceToFile( const char * filePath ) override;
    void loadPreferenceFromFile( const char * filePath ) override;

private:
    std::unique_ptr< class QtPreferencesImpl > pImpl_;
};
} // end namespace wgt
#endif//QT_PREFERENCES_HPP
