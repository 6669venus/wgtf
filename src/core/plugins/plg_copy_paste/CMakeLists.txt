CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_copy_paste )

INCLUDE( WGToolsCoreProject )
find_package(QT5)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	plg_copy_paste.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

SET( RESOURCE_FILES
  resources.qrc
)
source_group( "Resource Files" FILES ${RESOURCE_FILES} )

qt5_add_resources( COMPILED_RESOURCES ${RESOURCE_FILES} )
SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )

BW_ADD_TOOL_PLUGIN( plg_copy_paste SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_copy_paste PRIVATE
	core_generic_plugin
	core_qt_common
    Qt5::Core
    Qt5::Quick
)

BW_PROJECT_CATEGORY( plg_copy_paste "Plugins" )
