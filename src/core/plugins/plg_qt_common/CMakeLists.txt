CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_qt_common )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

set( RESOURCE_FILES
	resources/controls.qrc
	resources/icons.qrc
)

FILE( GLOB_RECURSE JS_SRCS *.js )

FILE( GLOB_RECURSE QML_SRCS *.qml *.js qmldir )

source_group( "Resource Files" FILES ${RESOURCE_FILES} )

qt5_add_resources( COMPILED_RESOURCES ${RESOURCE_FILES} )
SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )

SET( ALL_SRCS
	${QML_SRCS}
	${JS_SRCS}
	plugin_main.cpp
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
	generic_plugin
	qt_common
	qt_script
	ui_framework
)

# Link QT5 modules
qt5_use_modules( ${PROJECT_NAME} Core Gui QuickWidgets Quick )

FUNCTION( BW_COPY_OTHER_RESOURCES _PROJNAME _DIR_NAME )
	ADD_CUSTOM_COMMAND( TARGET ${_PROJNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/Apache License.txt" $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-Bold.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-BoldItalic.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-Italic.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-Regular.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
	)
ENDFUNCTION()

BW_COPY_OTHER_RESOURCES( ${PROJECT_NAME} generic_app )

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

