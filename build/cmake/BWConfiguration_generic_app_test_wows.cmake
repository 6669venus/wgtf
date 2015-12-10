SET( BW_SINGLETON_MANAGER_SUPPORT ON )
SET( NGT_SUPPORT ON )
SET( DEPLOY_QT_SUPPORT ON )

ADD_DEFINITIONS( -DEDITOR_ENABLED )
ADD_DEFINITIONS( -DALLOW_STACK_CONTAINER )

SET( TARGET_PLUGIN_APP_FOLDER_NAME generic_app_test_wows )
SET( BW_BUNDLE_NAME generic_app_wows )

INCLUDE( projects/core/WGToolsCore )
INCLUDE( projects/core/WGToolsCoreTests )

INCLUDE( projects/wows/WGToolsWoWS )
INCLUDE( projects/wows/WGToolsWoWSTests )

BW_GENERATE_DOC( WGToolsCore ${WG_TOOLS_SOURCE_DIR}/../doc/core/Doxyfile.in ${WG_TOOLS_SOURCE_DIR}/../doc/core )
