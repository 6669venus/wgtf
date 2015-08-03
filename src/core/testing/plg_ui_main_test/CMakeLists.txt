CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_ui_main_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( METADATA_IMPL
	pages/metadata/test_page.mpp
	pages/metadata/test_polymorphism.mpp
)
SOURCE_GROUP( "Page\\Metadata" FILES ${METADATA_IMPL} )

SET( PAGE_IMPL
	pages/test_page.hpp
	pages/test_page.cpp
	pages/test_polymorphism.hpp
	pages/test_polymorphism.cpp
)
SOURCE_GROUP( "Page\\Impl" FILES ${PAGE_IMPL} )

SET( INTERFACES
	interfaces/i_datasource.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACES} )

SET(RESOURCE_FILES
  controls.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

FILE(GLOB_RECURSE QML_SRCS *.qml *.js)


SET( ALL_SRCS
	${QML_SRCS}
	test_ui/test_ui.cpp
	test_ui/test_ui.hpp
	test_datasource.cpp
	test_datasource.hpp
	test_tree_item.cpp
	test_tree_item.hpp
	test_tree_model.cpp
	test_tree_model.hpp
	test_list_item.cpp
	test_list_item.hpp
	test_list_model.cpp
	test_list_model.hpp
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_ui_main_test ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${INTERFACES}
	${PAGE_IMPL}
	${METADATA_IMPL}
)

BW_TARGET_LINK_LIBRARIES( plg_ui_main_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_reflection
	reflection_objects_test
	core_command_system
	core_serialization
	core_ui_framework )

# Link QT5 modules
qt5_use_modules( plg_ui_main_test LINK_PRIVATE Core )

BW_PROJECT_CATEGORY( plg_ui_main_test "Plugins" )
