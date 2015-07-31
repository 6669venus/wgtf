# Additional include directories
INCLUDE_DIRECTORIES( ${NGT_SOURCE_DIR}/core/lib )
INCLUDE_DIRECTORIES( ${NGT_SOURCE_DIR}/core/third_party )

# Add include directories for remote build system header files
IF (BW_IS_REMOTE_ONLY AND  NOT ${BW_LINUX_CONN_TYPE} MATCHES "RSYNC" )
	INCLUDE_DIRECTORIES(BEFORE SYSTEM ${BW_REMOTE_INCLUDE_DIRS} )
ENDIF()


ADD_DEFINITIONS( -DBW_BUILD_PLATFORM="${BW_PLATFORM}" )

IF( BW_PLATFORM_WINDOWS )
    BW_ADD_COMPILE_FLAGS(
        /fp:fast  # Fast floating point model
    )

	# Windows version definitions
	ADD_DEFINITIONS(
		-DNTDDI_VERSION=0x05010000 # NTDDI_WINXP
		-D_WIN32_WINNT=0x0501   # Windows XP
		-D_WIN32_WINDOWS=0x0501
		-D_WIN32_IE=0x0600
	)

# Standard preprocessor definitions
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
	-D_CRT_NONSTDC_NO_DEPRECATE
	-D_CRT_SECURE_NO_DEPRECATE
	-D_CRT_NONSTDC_NO_DEPRECATE
	#-DD3DXFX_LARGEADDRESS_HANDLE # NOT YET FOR WOWP
)

ENDIF()

IF( NOT BW_NO_UNICODE )
	ADD_DEFINITIONS( -DUNICODE -D_UNICODE )
ENDIF()

# Remove /STACK:10000000 set by CMake. This value for stack size
# is very high, limiting the number of threads we can spawn.
# Default value used by Windows is 1MB which is good enough.
STRING( REGEX REPLACE "/STACK:[0-9]+" ""
	CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" )
