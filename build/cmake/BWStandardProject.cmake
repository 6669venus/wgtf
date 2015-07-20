# Additional include directories
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw/adapters )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/third_party )
#INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/third_party/directx/Include )
#INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/third_party/python/Include )
#INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/third_party/python/PC )
#INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/third_party/umbra/interface )

# Add include directories for remote build system header files
IF (BW_IS_REMOTE_ONLY AND  NOT ${BW_LINUX_CONN_TYPE} MATCHES "RSYNC" )
	INCLUDE_DIRECTORIES(BEFORE SYSTEM ${BW_REMOTE_INCLUDE_DIRS} )
ENDIF()

BW_ADD_COMPILE_FLAGS(
	/fp:fast  # Fast floating point model
)

ADD_DEFINITIONS( -DBW_BUILD_PLATFORM="${BW_PLATFORM}" )

#LINK_DIRECTORIES( ${BW_SOURCE_DIR}/third_party/nvtt/lib/${BW_COMPILER_TOKEN} )

IF( BW_PLATFORM_WINDOWS )

IF( NOT BW_IS_SERVER )
	# Windows version definitions
	ADD_DEFINITIONS(
		-DNTDDI_VERSION=0x05010000 # NTDDI_WINXP
		-D_WIN32_WINNT=0x0501   # Windows XP
		-D_WIN32_WINDOWS=0x0501
		-D_WIN32_IE=0x0600
	)
ENDIF()

# Standard preprocessor definitions
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
	-D_CRT_NONSTDC_NO_DEPRECATE
	-D_CRT_SECURE_NO_DEPRECATE
	-D_CRT_NONSTDC_NO_DEPRECATE
	#-DD3DXFX_LARGEADDRESS_HANDLE # NOT YET FOR WOWP
)

ENDIF()

IF ( MSVC AND BW_IS_REMOTE_ONLY )
BW_ADD_COMPILE_FLAGS(
	/u # Remove all predefined macros
	/FI"${CMAKE_BINARY_DIR}/gcc_defines.h" # Force include GCC defines
)
ENDIF()

# Tell python extensions that we are not building against python27.dll
# Ideally this definition should only be enabled in project which depend on python
# But since in BW nearly everything depends on python it's enabled for every project
IF( BW_PYTHON_DLL_SUPPORT OR BW_PYTHON_MODULE_SUPPORT )
	IF( BW_ARCH_32 )
		#LINK_DIRECTORIES( ${BW_SOURCE_DIR}/third_party/python/PCbuild )
	ELSEIF( BW_ARCH_64 )
		#LINK_DIRECTORIES( ${BW_SOURCE_DIR}/third_party/python/PCbuild/amd64 )
	ENDIF()
ELSE()
	ADD_DEFINITIONS( -DPy_BUILD_CORE )
ENDIF()

IF( NOT BW_NO_UNICODE )
	ADD_DEFINITIONS( -DUNICODE -D_UNICODE )
ENDIF()

# Remove /STACK:10000000 set by CMake. This value for stack size
# is very high, limiting the number of threads we can spawn.
# Default value used by Windows is 1MB which is good enough.
STRING( REGEX REPLACE "/STACK:[0-9]+" ""
	CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" )
