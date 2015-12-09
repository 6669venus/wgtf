SET( BW_SINGLETON_MANAGER_SUPPORT ON )
SET( NGT_SUPPORT ON )

ADD_DEFINITIONS( -DEDITOR_ENABLED )
ADD_DEFINITIONS( -DALLOW_STACK_CONTAINER )

SET( TARGET_PLUGIN_APP_FOLDER_NAME generic_app_test )
SET( BW_BUNDLE_NAME generic_app )

INCLUDE( projects/core/WGToolsCore )
INCLUDE( projects/core/WGToolsCoreTests )

INCLUDE( projects/core/WGToolsWoWS )
INCLUDE( projects/core/WGToolsWoWSTests )

BW_GENERATE_DOC( WGToolsCore ${WG_TOOLS_SOURCE_DIR}/../doc/core/Doxyfile.in ${WG_TOOLS_SOURCE_DIR}/../doc/core )
