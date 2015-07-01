CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( maya_plugin )

INCLUDE( BWMacros )
INCLUDE( BWMayaProject )
INCLUDE( BWProjectUserFile )


# Helper macro for adding maya app
MACRO( BW_ADD_MAYA_APP _PROJNAME _DIRNAME )
	ADD_LIBRARY( ${_PROJNAME} ${ARGN} )

	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		DEBUG_OUTPUT_NAME
		"${_PROJNAME}_d" )
	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		HYBRID_OUTPUT_NAME
		"${_PROJNAME}" )

	BW_SET_BINARY_DIR( ${_PROJNAME} "${BW_GAME_DIR}/${_DIRNAME}/${BW_PLATFORM}" )

	BW_PROJECT_CATEGORY( ${_PROJNAME} "Executables" )
ENDMACRO()

SET ( MAYA_DIR ${BW_SOURCE_DIR}/core/third_party/maya/${BW_MAYA_VERSION} )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${MAYA_DIR}/include )
INCLUDE_DIRECTORIES( ${MAYA_DIR}/include/Qt )

SET( APP_CODE_SRCS
	app/maya_dock_region.hpp
	app/maya_dock_region.cpp
	app/maya_menu.hpp
	app/maya_menu.cpp
	app/maya_menu_bar.hpp
	app/maya_menu_bar.cpp
	app/maya_plugin.cpp
	app/maya_plugin.hpp
	app/maya_tab_region.hpp
	app/maya_tab_region.cpp
	app/maya_tool_bar.hpp
	app/maya_tool_bar.cpp
	app/maya_window.hpp
	app/maya_window.cpp
	app/qwinhost.h
	app/qwinhost.cpp
	../generic_app/app/plugin_memory_allocator.hpp
	../generic_app/app/plugin_memory_allocator.cpp
	../generic_app/app/memory_plugin_context_creator.hpp
	../generic_app/app/memory_plugin_context_creator.cpp
)

SOURCE_GROUP( "App" FILES ${APP_CODE_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${APP_CODE_SRCS}
)

BW_ADD_MAYA_APP( maya_plugin ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( maya_plugin PRIVATE
	generic_plugin_manager
	wg_memory

	# external libraries
	shlwapi
	${MAYA_DIR}/lib/Foundation.lib
	${MAYA_DIR}/lib/OpenMaya.lib
	${MAYA_DIR}/lib/OpenMayaUI.lib
	${MAYA_DIR}/lib/OpenMayaAnim.lib
	${MAYA_DIR}/lib/QtCore4.lib
	${MAYA_DIR}/lib/QtGui4.lib
)

LIST( APPEND BW_LINKER_FLAGS /export:initializePlugin /export:uninitializePlugin )
SET_TARGET_PROPERTIES( maya_plugin PROPERTIES SUFFIX ".mll" )