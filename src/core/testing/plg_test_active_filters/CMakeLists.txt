CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
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

SET(RESOURCE_FILES
  resources.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	active_filters_test_view_model.cpp
	active_filters_test_view_model.hpp
	active_filters_test_view_model.mpp
	active_filters_test_panel.qml
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

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
	core_ui_framework )

# Link QT5 modules
qt5_use_modules( plg_test_active_filters LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_test_active_filters "Plugins" )
