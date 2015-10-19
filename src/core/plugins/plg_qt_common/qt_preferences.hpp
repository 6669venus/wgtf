#ifndef QT_PREFERENCES_HPP
#define QT_PREFERENCES_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_ui_framework/i_preferences.hpp"


#include <unordered_map>


class QtPreferences : public Implements< IPreferences >
{
public:
	QtPreferences( IDefinitionManager & definitionManger );
	~QtPreferences();
	GenericObjectPtr & getPreference( const char * key ) override;

	void savePrferences();
	void loadPreferences();

private:
	IDefinitionManager & definitionManager_;
	std::unordered_map< std::string, GenericObjectPtr > preferences_;
};

#endif//QT_PREFERENCES_HPP
