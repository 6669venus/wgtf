CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_test_active_filters )

INCLUDE( BWStandardProject )
find_package(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	active_filters_test_view_model.cpp
	active_filters_test_view_model.hpp
	active_filters_test_view_model.mpp
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_test_active_filters resources/plg_test_active_filters)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_test_active_filters SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_test_active_filters PRIVATE
	core_data_model
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_reflection
	core_ui_framework
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Widgets
)

BW_PROJECT_CATEGORY( plg_test_active_filters "Plugins" )
