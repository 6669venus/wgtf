CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_wgs_core )

INCLUDE( BWStandardProject )

REMOVE_DEFINITIONS( -WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# this requires WGS Excalibur codebase with 'wgs make gamecentralutils' successfully completed
SET( WGS_OUTPUT_DIR "${BW_SOURCE_DIR}/../../javelin/output/win64_vs2013/" )

#SET( WGS_OUTPUT_BIN_DIR "${WGS_OUTPUT_DIR}/bin" )
#SET( WGS_OUTPUT_LIB_DIR "${WGS_OUTPUT_DIR}/lib" )
#SET( WGS_BINARY_POSTFIX "w64r" ) # must match the output directory for the binary (release == w64r, debug == w64d)

# Link against the despair engine exported lib
ADD_LIBRARY( despair_engine_lib STATIC IMPORTED )
SET_TARGET_PROPERTIES( despair_engine_lib PROPERTIES IMPORTED_LOCATION_DEBUG ${WGS_OUTPUT_DIR}/debug/lib/engine_w64d.lib )
SET_TARGET_PROPERTIES( despair_engine_lib PROPERTIES IMPORTED_LOCATION_RELEASE ${WGS_OUTPUT_DIR}/release/lib/engine_w64r.lib )
SET_TARGET_PROPERTIES( despair_engine_lib PROPERTIES IMPORTED_LOCATION_HYBRID ${WGS_OUTPUT_DIR}/release/lib/engine_w64r.lib )

ADD_LIBRARY( despair_graphics_lib STATIC IMPORTED )
SET_TARGET_PROPERTIES( despair_graphics_lib PROPERTIES IMPORTED_LOCATION_DEBUG ${WGS_OUTPUT_DIR}/debug/lib/graphics_w64d.lib )
SET_TARGET_PROPERTIES( despair_graphics_lib PROPERTIES IMPORTED_LOCATION_RELEASE ${WGS_OUTPUT_DIR}/release/lib/graphics_w64r.lib )
SET_TARGET_PROPERTIES( despair_graphics_lib PROPERTIES IMPORTED_LOCATION_HYBRID ${WGS_OUTPUT_DIR}/release/lib/graphics_w64r.lib )

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/../../javelin/source/PublicHeaders)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( INTERFACES_SRCS
	interfaces/i_engine_adapter.hpp
	i_viewport.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACES_SRCS} )

SET( ALL_SRCS
	engine_adapter.h
	engine_adapter.cpp

	despair_class.h
	despair_class.cpp
	despair_property.h
	despair_property.cpp

	game_loader.cpp
	game_loader.h
	game_type_provider.cpp
	game_type_provider.h
	game_common.h
	game_filesystem.cpp
	game_filesystem.h
	game_viewport.cpp
	game_viewport.h
	
	plugin_main.cpp

	${INTERFACES_SRCS})
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_wgs_core ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_wgs_core PRIVATE
	reflection
	generic_plugin
	command_system
	serialization
	despair_engine_lib
	despair_graphics_lib
)

BW_PROJECT_CATEGORY( plg_wgs_core "Plugins" )

FUNCTION( WGS_COPY_DESPAIR_DLLS _PROJNAME _DIR_NAME )
	
	ADD_CUSTOM_COMMAND(	TARGET ${_PROJNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo Copying $<CONFIGURATION> Despair libraries...

		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/gamecentralutils_w64r.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/gamecentralutils_w64r.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/engine_w64r.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/engine_w64r.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/javelin_w64r.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/javelin_w64r.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/umbraoptimizer64.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/bwentity64.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/graphics_w64r.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==0 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/release/bin/graphics_w64r.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/

		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/gamecentralutils_w64d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/gamecentralutils_w64d.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/engine_w64d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/engine_w64d.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/javelin_w64d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/javelin_w64d.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/umbraoptimizer64_d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/bwentity64_d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/graphics_w64d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND if $<CONFIG:Debug>==1 ${CMAKE_COMMAND} -E copy_if_different ${WGS_OUTPUT_DIR}/debug/bin/graphics_w64d.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
	)
ENDFUNCTION()
	
IF(BW_IS_GENERIC_APP_TEST)
	WGS_COPY_DESPAIR_DLLS( ${PROJECT_NAME} generic_app )
ENDIF()