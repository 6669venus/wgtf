CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_qt_common )

INCLUDE( WGToolsCoreProject )
INCLUDE( BWQtLibrary )

BW_QT_ADD_QRC(WGControls resources/WGControls)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
    ${ALL_SRCS}
    plg_qt_common.cpp
    qt_framework.cpp
    qt_framework.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
    ${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
    ${BLOB_SRCS}
    ${RESOURCE_FILES}
    ${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_common
    core_generic_plugin
    core_qt_common
    core_qt_script
    core_ui_framework
)

# Link QT5 modules
qt5_use_modules( ${PROJECT_NAME} LINK_PRIVATE Core Gui QuickWidgets Quick )

FILE( GLOB resFiles resources/fonts/* )
BW_DEPLOY_RESOURCES( .. "${resFiles}" )

IF( BW_IS_GENERIC_APP_TEST OR BW_IS_QT_TOOLS )
    BW_DEPLOY_QT( generic_app )
ENDIF()

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )


