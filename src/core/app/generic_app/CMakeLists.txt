CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( generic_app )

INCLUDE( BWStandardProject )
INCLUDE( BWProjectUserFile )

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR} )

SET( APP_CODE_SRCS
	app/main.cpp
	app/plugin_memory_allocator.hpp
	app/plugin_memory_allocator.cpp
	app/memory_plugin_context_creator.hpp
	app/memory_plugin_context_creator.cpp
)
SOURCE_GROUP( "App" FILES ${APP_CODE_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${APP_CODE_SRCS}
)
BW_ADD_TOOL_EXE( generic_app ${TARGET_PLUGIN_APP_FOLDER_NAME} WIN32 ${BLOB_SRCS} )


BW_TARGET_LINK_LIBRARIES( generic_app PRIVATE
	generic_plugin_manager
	wg_memory

	# external libraries
	shlwapi
)

MACRO( BW_DEPLOY_RESOURCES _TARGET_DIR _RESOURCES )
    FOREACH( resFile ${_RESOURCES} )
        ADD_CUSTOM_COMMAND( TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${resFile}" $<TARGET_FILE_DIR:${PROJECT_NAME}>/${_TARGET_DIR}
            VERBATIM 
	    )
    ENDFOREACH()
ENDMACRO()

FILE(GLOB pluginConfigFiles plugins.txt plugins_ui.txt)

BW_DEPLOY_RESOURCES( plugins "${pluginConfigFiles}" )

BW_PROJECT_CATEGORY( generic_app "Executables" )

