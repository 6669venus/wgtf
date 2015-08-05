CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( qt_script )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
INCLUDE( BWQtLibrary )

SET( ALL_SRCS
	qobject_qt_type_converter.hpp
	wg_list_iterator.cpp
	wg_list_iterator.hpp
	qt_scripting_engine.cpp
	qt_scripting_engine.hpp
	qt_script_object.cpp
	qt_script_object.hpp
	script_qt_type_converter.cpp
	script_qt_type_converter.hpp
	collection_qt_type_converter.cpp
	collection_qt_type_converter.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( qt_script ${BLOB_SRCS}
)

# Link QT5 modules
qt5_use_modules( qt_script LINK_PRIVATE Core Gui Widgets Quick )

INCLUDE_DIRECTORIES(${Qt5Core_PRIVATE_INCLUDE_DIRS})

BW_PROJECT_CATEGORY( qt_script "NGT Libs" )

