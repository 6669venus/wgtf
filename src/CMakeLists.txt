# Root level BigWorld CMake script. CMake should always be invoked at this
# level rather than on specific subdirectories. Specify what projects you
# want to generate using the BW_CMAKE_TARGET option, e.g.
#
# cmake -DBW_CMAKE_TARGET=client
# 
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )

# Set the BigWorld source directory
SET( BW_SOURCE_DIR ${CMAKE_SOURCE_DIR} )

# Set serach location for CMake include files
SET( CMAKE_MODULE_PATH "${BW_SOURCE_DIR}/../build/cmake" )

# Ensure the BW_CMAKE_TARGET has been set
IF( NOT BW_CMAKE_TARGET )
	MESSAGE( FATAL_ERROR "BW_CMAKE_TARGET is not set, either configure CMake by running bigworld/src/build/bigworld_cmake.bat or specify BW_CMAKE_TARGET when invoking CMake." )
ENDIF()

# Override the default CMake configs: Debug Release MinSizeRel RelWithDebInfo
# This must appear before ENABLE_LANGUAGE to override the defaults
SET( CMAKE_CONFIGURATION_TYPES Debug;Hybrid
	CACHE STRING "Semicolon separated list of supported configuration types"
	FORCE )

# Enable C and C++ to force compiler detection so we can determine 32 or 64 bit
ENABLE_LANGUAGE( C )
ENABLE_LANGUAGE( CXX )

# Ensure we have a functioning compiler. Work around CMake failing to detect
# that the compiler is unknown, do this by making sure there's some flags
IF( NOT CMAKE_CXX_FLAGS )
	MESSAGE( FATAL_ERROR "CMake failed to find a valid compiler!" )
ENDIF()

# Determine platform
IF( WIN32 )
	SET( BW_PLATFORM_WINDOWS ON )
	IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
		SET( BW_PLATFORM "win32" )
		SET( BW_ARCH_32 ON )
	ELSEIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		SET( BW_PLATFORM "win64" )
		SET( BW_ARCH_64 ON )
	ELSE()
		MESSAGE( FATAL_ERROR "Only 32 and 64 bit builds are supported." )
	ENDIF()
ELSE()
	MESSAGE( FATAL_ERROR "Only Windows builds are currently supported." )
ENDIF()

# Do compiler specific configuration. These are generally used to link
# against .libs for a specific version of MSVC.
IF( MSVC9 )
	SET( BW_COMPILER_TOKEN "vc9" )
ELSEIF( MSVC10 )
	SET( BW_COMPILER_TOKEN "vc10" )
ELSEIF( MSVC11 )
	SET( BW_COMPILER_TOKEN "vc11" )
ELSEIF( MSVC12 )
	SET( BW_COMPILER_TOKEN "vc12" )
ELSE()
	MESSAGE( FATAL_ERROR "Compiler '${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}' not currently supported by BigWorld." )
ENDIF()

MESSAGE( STATUS "Compiler: ${BW_COMPILER_TOKEN}" )
MESSAGE( STATUS "Platform: ${BW_PLATFORM}" )

PROJECT( ${BW_CMAKE_TARGET}_${BW_PLATFORM} )

# Binary output location
GET_FILENAME_COMPONENT( BW_GAME_DIR "${PROJECT_SOURCE_DIR}/../bin" REALPATH )
GET_FILENAME_COMPONENT( BW_RESOURCE_DIR "${PROJECT_SOURCE_DIR}/../res" REALPATH )

# Include useful macros
INCLUDE( BWMacros )

# Detect clang-cl
IF( MSVC AND ${CMAKE_C_COMPILER_ID} MATCHES "Clang" )
	SET( CLANG_CL 1 )
ENDIF()

# Set compiler and linker options
INCLUDE( BWCompilerAndLinkerOptions )

# Options
OPTION( BW_UNIT_TESTS_ENABLED "Enable unit tests" ON )

# Discover available BigWorld projects to generate. These are defined by 
# BWConfiguration_<name>.cmake files in the cmake module path.
FILE( GLOB _target_cmake_files "${CMAKE_MODULE_PATH}/BWConfiguration_*.cmake" )
FOREACH( _filename ${_target_cmake_files} )
	GET_FILENAME_COMPONENT( _target_name ${_filename} NAME_WE )
	STRING( REPLACE "BWConfiguration_" "" _target_name ${_target_name} )
	SET( BW_AVAILABLE_TARGETS ${BW_AVAILABLE_TARGETS} ${_target_name} )
ENDFOREACH()

# What are we generating?
LIST( FIND BW_AVAILABLE_TARGETS ${BW_CMAKE_TARGET} _exists )
IF( NOT ${_exists} EQUAL -1 )
	MESSAGE( STATUS "Generating BigWorld ${BW_CMAKE_TARGET}..." )
ELSE()
	MESSAGE( FATAL_ERROR "Unknown BigWorld CMake target '${BW_CMAKE_TARGET}'." )
ENDIF()

SET( BW_CMAKE_TARGET ${BW_CMAKE_TARGET}
	CACHE STRING "Type of BigWorld project to generate" )

# Set BW_IS_<name> variables
FOREACH( _targetname ${BW_AVAILABLE_TARGETS} )
	STRING( TOUPPER ${_targetname} _upper_targetname )
	SET( BW_IS_${_upper_targetname} OFF )
ENDFOREACH()

STRING( TOUPPER ${BW_CMAKE_TARGET} _upper_targetname )
# If this is a blob build, strip the suffix off the blob
STRING( REGEX REPLACE "_BLOB$" "" _upper_targetname "${_upper_targetname}" )
SET( BW_IS_${_upper_targetname} ON )

# Solution folders: http://athile.net/library/blog/?p=288
SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

# Default feature support, may be overridden by target configuration
SET( BW_PCH_SUPPORT ON )
SET( BW_ASSET_PIPELINE_SUPPORT OFF )
SET( USE_MEMHOOK OFF )
SET( BW_FMOD_SUPPORT ON )
SET( BW_PYTHON_MODULE_SUPPORT OFF )
SET( BW_PYTHON_DLL_SUPPORT OFF )
SET( BW_SCALEFORM_SUPPORT OFF )
SET( BW_SPEEDTREE_SUPPORT ON )
SET( BW_UMBRA_SUPPORT ON )
SET( BW_SINGLETON_MANAGER_SUPPORT OFF )
SET( NGT_SUPPORT OFF )

# Include target specific configuration
INCLUDE( BWConfiguration_${BW_CMAKE_TARGET} )

# blob specific configuration
IF( BW_BLOB_CONFIG )
	ADD_DEFINITIONS( -DBW_BLOB_CONFIG=1 )
	IF( MSVC )
		IF( BW_PLATFORM STREQUAL "win64" )
			SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
		ENDIF()
	ENDIF()
ENDIF()

# Add feature flags
IF( BW_PCH_SUPPORT )
	ADD_DEFINITIONS( -DPCH_SUPPORT=1 )
ENDIF()
IF( BW_ASSET_PIPELINE_SUPPORT )
 	ADD_DEFINITIONS( -DASSET_PIPELINE_SUPPORT=1 )
ENDIF()
IF( USE_MEMHOOK )
	ADD_DEFINITIONS( -DUSE_MEMHOOK=1 )
ENDIF()
IF( BW_FMOD_SUPPORT )
	ADD_DEFINITIONS( -DFMOD_SUPPORT=1 )
	SET( FMOD_INCLUDE_DIRS
		"${BW_SOURCE_DIR}/third_party/fmod/api/inc"
		"${BW_SOURCE_DIR}/third_party/fmod/fmoddesignerapi/api/inc" )
ENDIF()
IF( BW_PYTHON_MODULE_SUPPORT )
	# Build as *.pyd
	ADD_DEFINITIONS( -DBWCLIENT_AS_PYTHON_MODULE=1
		-DPYMODULE )
ENDIF()
IF( BW_PYTHON_DLL_SUPPORT )
	# Use existing python*.dll
	ADD_DEFINITIONS( 
		-DPYTHON_IMPORT
	)
ENDIF()
IF( BW_PYTHON_DLL_SUPPORT AND BW_PYTHON_MODULE_SUPPORT )
	MESSAGE( FATAL_ERROR "Cannot have BW_PYTHON_DLL_SUPPORT and BW_PYTHON_MODULE_SUPPORT enabled at the same time." )
ENDIF()
IF( BW_SCALEFORM_SUPPORT )
	ADD_DEFINITIONS( -DSCALEFORM_SUPPORT=1 )
ENDIF()
IF( BW_SPEEDTREE_SUPPORT )
	ADD_DEFINITIONS( -DSPEEDTREE_SUPPORT=1 )
ENDIF()
IF( BW_UMBRA_SUPPORT )
	ADD_DEFINITIONS( -DUMBRA_ENABLE=1 )
ENDIF()
IF( BW_SINGLETON_MANAGER_SUPPORT )
	ADD_DEFINITIONS( -DENABLE_SINGLETON_MANAGER )
ENDIF()
IF( NGT_SUPPORT )
	ADD_DEFINITIONS( -DENABLE_NGT_SUPPORT )
ENDIF()

# Setup CODE_INLINE for Hybrid and Consumer Release
SET_PROPERTY( DIRECTORY APPEND PROPERTY
	COMPILE_DEFINITIONS_HYBRID
	CODE_INLINE )
SET_PROPERTY( DIRECTORY APPEND PROPERTY
	COMPILE_DEFINITIONS_CONSUMER_RELEASE
	CODE_INLINE )

IF( CMAKE_BUILD_TYPE )
	MESSAGE( STATUS "Build configuration: ${CMAKE_BUILD_TYPE}" )
ELSE()
	MESSAGE( STATUS "Build configurations: ${CMAKE_CONFIGURATION_TYPES}" )
ENDIF()

# Add libraries
ARRAY2D_BEGIN_LOOP( _islooping "${BW_LIBRARY_PROJECTS}" 2 "libname;libpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding library: ${libname} from ${libpath}" )
		
		ADD_SUBDIRECTORY( ${libpath} )
		
		# Prevent libraries from being part of the remote build, as
		# dependencies are handled by the remote build
		IF ( BW_IS_REMOTE_ONLY )
			SET_TARGET_PROPERTIES( ${libname} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1 )
		ENDIF()
		
		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()

# Add executables
INCLUDE( BWProjectUserFile )
ARRAY2D_BEGIN_LOOP( _islooping "${BW_BINARY_PROJECTS}" 2 "projname;projpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding executable: ${projname} from ${projpath}" )
		ADD_SUBDIRECTORY( ${projpath} )
		
		IF ( BW_IS_REMOTE_ONLY )
			# Prevent libraries from being part of the remote build, as
			# dependencies are handled by the remote build
			SET_TARGET_PROPERTIES( ${projname} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1 )
		ELSE()
			# Always set the working directory to bin for every project
			BW_SET_WORKING_DIRECTORY( ${projname} ${projpath} "$(TargetDir)" )
		ENDIF()
		
		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()
IF( BW_IS_REMOTE_ONLY )
	# Add a build all option for the server
	# NOTE: This is not excluded from all or default to allow
	#    the Visual studio solution to build the server.
	BW_ADD_EXECUTABLE( BUILD_SERVER _NON_EXISTANT_FILE.cpp )
	SET_SOURCE_FILES_PROPERTIES( _NON_EXISTANT_FILE.cpp PROPERTIES GENERATED TRUE )
	BW_PROJECT_CATEGORY( BUILD_SERVER "Server Commands" )

	# Add cleanup of post generation updater
	STRING( REPLACE "/" "\\" CMAKE_BINARY_DIR_WINDOWS ${CMAKE_BINARY_DIR} )
	FILE( APPEND "${CMAKE_BINARY_DIR}/_update_projs.bat" "\necho @ > \"${CMAKE_BINARY_DIR_WINDOWS}/_update_projs.bat\"" )
ENDIF()

# Add plugins
ARRAY2D_BEGIN_LOOP( _islooping "${BW_PLUGIN_PROJECTS}" 2 "plugname;plugpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding plugin: ${plugname} from ${plugpath}" )
		ADD_SUBDIRECTORY( ${plugpath} )
			
		# Always generate debug symbols, even in Release builds
		IF( MSVC )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_HYBRID "/DEBUG" )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_CONSUMER_RELEASE "/DEBUG" )
		ENDIF( MSVC )
		
		# Always set the working directory to bin for every project
		BW_SET_WORKING_DIRECTORY( ${plugname} ${plugpath} "$(TargetDir)" )
		
		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()

