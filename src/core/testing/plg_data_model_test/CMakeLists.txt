CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_data_model_test )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
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
	custom_model_interface_test.cpp
	custom_model_interface_test.hpp
	plg_data_model_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_data_model_test SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_data_model_test PRIVATE
	core_generic_plugin
	core_reflection
	core_ui_framework )

# Link QT5 modules
qt5_use_modules( plg_data_model_test LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_data_model_test "Plugins" )
