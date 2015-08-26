CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_example_listfilter_bug )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

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
	bugged_list_view_model.cpp
	bugged_list_view_model.hpp
	bugged_list_view_model.mpp
	bugged_list_filter_panel.qml
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_example_listfilter_bug ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_example_listfilter_bug PRIVATE
	core_data_model
	core_generic_plugin
	core_reflection )

# Link QT5 modules
qt5_use_modules( plg_example_listfilter_bug LINK_PRIVATE Core )

BW_PROJECT_CATEGORY( plg_example_listfilter_bug "Plugins" )
