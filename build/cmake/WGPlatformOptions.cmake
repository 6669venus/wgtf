# Enable C and C++ to force compiler detection so we can determine 32 or 64 bit
ENABLE_LANGUAGE( C )
ENABLE_LANGUAGE( CXX )

# Determine platform
IF( WIN32 )
	SET( BW_PLATFORM_WINDOWS ON )
	IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
		SET( BW_PLATFORM "win32" )
	ELSEIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		SET( BW_PLATFORM "win64" )
	ENDIF()
ELSEIF( APPLE )
	SET( BW_PLATFORM_MAC ON )
	SET( BW_PLATFORM "mac" )
	SET( BW_COMPILER_TOKEN "clang" )
	# DAVA team use C++14 feature, and as DAVA is only one team that compile WGTF for macos,
	# there is no reason to write some complicated logic to resolve feature subset.
	# I simple commented this line, because DAVA team use inherited compiler flags for WGTF. 
	#SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++" )
ELSE()
	MESSAGE( FATAL_ERROR "Only Windows builds are currently supported." )
ENDIF()

IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
	SET( BW_ARCH_32 ON )
ELSEIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET( BW_ARCH_64 ON )
ELSE()
	MESSAGE( FATAL_ERROR "Only 32 and 64 bit builds are supported." )
ENDIF()
