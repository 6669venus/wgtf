#ifndef QT_PREFERENCES_HPP
#define QT_PREFERENCES_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include <unordered_map>

class ISerializationManager;
class IFileSystem;
class IQtFramework;

class QtPreferences : public Implements< IPreferences >
{
public:
	QtPreferences( IDefinitionManager & definitionManger, 
				   ISerializationManager & serializationManager, 
				   IFileSystem & fileSystem,
				   IMetaTypeManager & metaTypeManager, 
                   IQtFramework & uiFramework);
	~QtPreferences();
	GenericObjectPtr & getPreference( const char * key ) override;
    void registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener ) override;
    void deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener ) override;
    void savePreferenceToFile( const char * filePath ) override;
    void loadPreferenceFromFile( const char * filePath ) override;

	void savePrferences();
	void loadPreferences();

private:
	IDefinitionManager & definitionManager_;
	ISerializationManager & serializationManager_;
	IFileSystem & fileSystem_;
	IMetaTypeManager & metaTypeManager_;
    IQtFramework & qtFramework_;
	std::unordered_map< std::string, GenericObjectPtr > preferences_;
    PreferencesListeners listeners_;
};

#endif//QT_PREFERENCES_HPP
