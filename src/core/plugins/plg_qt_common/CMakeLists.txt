CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_qt_common )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

BW_QT_ADD_QRC(WGControls resources/WGControls)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
    ${ALL_SRCS}
    plg_qt_common.cpp
    qt_copy_paste_manager.cpp
    qt_copy_paste_manager.hpp
	qt_framework.cpp
	qt_framework.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
    ${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
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
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickWidgets
)

FILE( GLOB resFiles resources/fonts/* )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( .. "${resFiles}" )

IF( BW_IS_GENERIC_APP_TEST OR BW_IS_QT_TOOLS )
    BW_DEPLOY_QT( generic_app )
ENDIF()

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )


