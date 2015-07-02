SET( BW_SINGLETON_MANAGER_SUPPORT ON )
SET( NGT_SUPPORT ON )

# Find Maya
SET( MAYA_DEVKIT_DIR ${BW_SOURCE_DIR}/core/third_party/maya/${MAYA_VERSION} )
SET( MAYA_QT_HEADER_DIR ${MAYA_DEVKIT_DIR}/qt/include )
SET( MAYA_INCLUDE_DIRS ${MAYA_DEVKIT_DIR}/include)

FIND_PACKAGE( Maya ${MAYA_VERSION} REQUIRED )

# Help FindQt4.cmake find the Maya Qt
SET( ENV{QTDIR} ${MAYA_LOCATION} )
SET(QT_HEADERS_DIR ${MAYA_QT_HEADER_DIR})

FIND_PATH(QT_QTCORE_INCLUDE_DIR QtCore
        HINTS ${QT_HEADERS_DIR}
        PATH_SUFFIXES QtCore
        NO_DEFAULT_PATH
)

FIND_PACKAGE( Qt4 ${MAYA_QT_VERSION_SHORT} REQUIRED QtCore QtGui )

# load the Qt tools
INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS( -DEDITOR_ENABLED )
ADD_DEFINITIONS( -DALLOW_STACK_CONTAINER )

# Hack to get Qt binary copy workign while building via this configuration file.
SET(BW_IS_QT_TOOLS ON)
SET(BW_IS_GENERIC_APP_TEST ON)

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw/lib )

INCLUDE_DIRECTORIES( ${MAYA_DEVKIT_DIR}/include )
INCLUDE_DIRECTORIES( ${MAYA_QT_HEADER_DIR} )

SET( TARGET_PLUGIN_APP_FOLDER_NAME maya_plugin )


SET( BW_LIBRARY_PROJECTS	

	#WG Systems
	wg_types			core/lib/wg_types 
	wg_memory			core/lib/wg_memory

	#NGT Systems
	variant				core/lib/variant
	ngt_core_common		core/lib/ngt_core_common
	dependency_system	core/lib/dependency_system
	ui_framework        core/lib/ui_framework

	#Tools Common
	logging				core/lib/logging
	generic_plugin		core/lib/generic_plugin
	generic_plugin_manager	core/lib/generic_plugin_manager
)

SET( BW_BINARY_PROJECTS	
	# Apps
	ngt_maya_plugin			core/app/maya_plugin
)
