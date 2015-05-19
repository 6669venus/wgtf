# - Find BigWorld-bundled OpenSSL installation
# Try to find the OpenSSL libraries bundled with BigWorld
# Once done, this will define
#	BWOPENSSL_FOUND - BigWorld-bundled OpenSSL is present and compiled
#	BWOPENSSL_INCLUDE_DIRS - The BigWorld-bundled OpenSSL include directories
#	BWOPENSSL_LIBRARIES - The libraries needed to use the BigWorld-bundled OpenSSL

# Based on FindScaleformSDK.cmake, which is
# based on http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries
# and FindALSA.cmake, which is apparently current best-practice

# Hide all of the OpenSSL build madness.
IF( MSVC )
	# Note: Remote server build windows builds
	# due to .a files not existing at this point in time.
	IF( BW_ARCH_32 )
		SET( BWOPENSSL_INC_DIR_NAME inc32 )
		SET( BWOPENSSL_LIB_DIR_NAME out32 )
	ELSE()
		SET( BWOPENSSL_INC_DIR_NAME inc64 )
		SET( BWOPENSSL_LIB_DIR_NAME out64 )
	ENDIF()
	# The 64-bit libraries are *32.lib as well.
	SET( BWOPENSSL_CRYPTO_LIB_NAME libeay32.lib )
	SET( BWOPENSSL_SSL_LIB_NAME ssleay32.lib )
ELSE()
	SET( BWOPENSSL_INC_DIR_NAME include )
	SET( BWOPENSSL_LIB_DIR_NAME )
	SET( BWOPENSSL_CRYPTO_LIB_NAME libcrypto.a )
	SET( BWOPENSSL_SSL_LIB_NAME libssl.a )
ENDIF()

FIND_PATH( BWOPENSSL_ROOT_DIR 
	${BWOPENSSL_INC_DIR_NAME}/openssl/opensslconf.h
	PATHS	${BW_SOURCE_DIR}/third_party/openssl
)

# BWOpenSSL Include dir
FIND_PATH( BWOPENSSL_INCLUDE_DIR
	openssl/opensslv.h "${BWOPENSSL_ROOT_DIR}/${BWOPENSSL_INC_DIR_NAME}"
)

# Extract the version from OPENSSL_VERSION_TEXT in openssl/opensslv.h
IF( BWOPENSSL_INCLUDE_DIR AND EXISTS "${BWOPENSSL_INCLUDE_DIR}/openssl/opensslv.h" )
	FILE( STRINGS "${BWOPENSSL_INCLUDE_DIR}/openssl/opensslv.h"
		_BWOPENSSL_VERSION_STR
		REGEX "^#define[\t ]+OPENSSL_VERSION_TEXT[\t ]+\"OpenSSL [0-9.a-z]+ .*\""
	)
	STRING( REGEX REPLACE "^.*OpenSSL ([^\ ]*) .*$" "\\1"
		BWOPENSSL_VERSION_STRING ${_BWOPENSSL_VERSION_STR}
	)
	UNSET( _BWOPENSSL_VERSION_STR )
ENDIF()

# BWOpenSSL library paths
FIND_LIBRARY( BWOPENSSL_LIBRARY_CRYPTO
	${BWOPENSSL_CRYPTO_LIB_NAME} ${BWOPENSSL_ROOT_DIR}/${BWOPENSSL_LIB_DIR_NAME}
)

FIND_LIBRARY( BWOPENSSL_LIBRARY_SSL
	${BWOPENSSL_SSL_LIB_NAME} ${BWOPENSSL_ROOT_DIR}/${BWOPENSSL_LIB_DIR_NAME}
)
INCLUDE( FindPackageHandleStandardArgs )

FIND_PACKAGE_HANDLE_STANDARD_ARGS( BWOpenSSL
	REQUIRED_VARS
		# Top of BWOpenSSL tree
		BWOPENSSL_ROOT_DIR 
		# Headers
		BWOPENSSL_INCLUDE_DIR
		# Libraries
		BWOPENSSL_LIBRARY_CRYPTO
		BWOPENSSL_LIBRARY_SSL
	VERSION_VAR BWOPENSSL_VERSION_STRING
)

IF( BWOPENSSL_FOUND )
	SET( BWOPENSSL_INCLUDE_DIRS
		${BWOPENSSL_INCLUDE_DIR}
	)

	SET( BWOPENSSL_LIBRARIES
		${BWOPENSSL_LIBRARY_CRYPTO}
		${BWOPENSSL_LIBRARY_SSL}
	)

ENDIF()

MARK_AS_ADVANCED( 
	BWOPENSSL_INCLUDE_DIR
	BWOPENSSL_LIBRARY_CRYPTO
	BWOPENSSL_LIBRARY_SSL
)
