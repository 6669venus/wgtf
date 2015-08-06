CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )

INCLUDE( BWMacros )
INCLUDE( BWMayaProject )
INCLUDE( BWProjectUserFile )

IF( MAYA_THIRD_PARTY_NOT_FOUND )
    MESSAGE( "Maya third party directory is not found, skip building Maya plugin!" )
    RETURN()
ENDIF()

PROJECT( maya_plugin )

# Helper macro for adding maya app
MACRO( BW_ADD_MAYA_APP _PROJNAME _DIRNAME )
	ADD_LIBRARY( ${_PROJNAME} ${ARGN} )

	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		DEBUG_OUTPUT_NAME
		"${_PROJNAME}_d" )
	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		HYBRID_OUTPUT_NAME
		"${_PROJNAME}" )

	BW_SET_BINARY_DIR( ${_PROJNAME} "${WG_TOOLS_BIN_DIR}/${_DIRNAME}/${BW_PLATFORM}" )

	BW_PROJECT_CATEGORY( ${_PROJNAME} "Executables" )
ENDMACRO()


SET( maya_plugin_HEADERS
	app/maya_dock_region.hpp
	app/maya_menu.hpp
	app/maya_menu_bar.hpp
	app/maya_plugin.hpp
	app/maya_tab_region.hpp
	app/maya_tool_bar.hpp
	app/maya_window.hpp
	app/ngt_event_loop.hpp
	app/ngt_widget_helper.hpp
	app/qwinhost.h
)

QT4_WRAP_CPP( maya_plugin_MOC_SRCS ${maya_plugin_HEADERS} )

SET( maya_plugin_SRCS
	${maya_plugin_HEADERS}
	${maya_plugin_MOC_SRCS}
	app/maya_dock_region.cpp
	app/maya_menu.cpp
	app/maya_menu_bar.cpp
	app/maya_plugin.cpp
	app/maya_tab_region.cpp
	app/maya_tool_bar.cpp
	app/maya_window.cpp
	app/ngt_event_loop.cpp
	app/ngt_widget_helper.cpp
	app/qwinhost.cpp
	../generic_app/app/plugin_memory_allocator.hpp
	../generic_app/app/plugin_memory_allocator.cpp
	../generic_app/app/memory_plugin_context_creator.hpp
	../generic_app/app/memory_plugin_context_creator.cpp
)

SOURCE_GROUP( "Sources" FILES ${maya_plugin_SRCS} )
SOURCE_GROUP( "Generated Sources" FILES ${maya_plugin_MOC_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${maya_plugin_SRCS}
)

BW_ADD_MAYA_APP( maya_plugin ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( maya_plugin PRIVATE
	core_common
	core_generic_plugin_manager
	wg_memory

	# external libraries
	shlwapi
	${MAYA_Foundation_LIBRARY}
	${MAYA_OpenMaya_LIBRARY}
	${MAYA_OpenMayaUI_LIBRARY}
	${MAYA_OpenMayaAnim_LIBRARY}
	${QT_LIBRARIES}
)

FILE(GLOB pluginConfigFiles plugins_maya.txt)

BW_DEPLOY_RESOURCES( plugins "${pluginConfigFiles}" )

# this is a macro defined in FindMaya that sets up our plugin with standard Maya build settings
MAYA_SET_PLUGIN_PROPERTIES( maya_plugin )
