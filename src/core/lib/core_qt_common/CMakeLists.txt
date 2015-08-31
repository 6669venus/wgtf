CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( core_qt_common )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
INCLUDE( BWQtLibrary )

ADD_DEFINITIONS( -DQT_DECLARATIVE_DEBUG )
ADD_DEFINITIONS( -DQT_QML_DEBUG )

find_package( Qt5Quick REQUIRED )

SET( DATA_MODEL_SRCS
	models/data_change_notifier.cpp
	models/data_change_notifier.hpp
	models/qt_model_helpers.cpp
	models/qt_model_helpers.hpp
	models/qt_model_macros.hpp
	models/wg_tree_model.cpp
	models/wg_tree_model.hpp
	models/wg_list_model.cpp
	models/wg_list_model.hpp
	models/role_provider.cpp
	models/role_provider.hpp
) 
SOURCE_GROUP( "DataModel" FILES ${DATA_MODEL_SRCS} )

SET( DATA_MODEL_ADAPTERS_SRCS
	models/adapters/i_list_adapter.cpp
	models/adapters/i_list_adapter.hpp
	models/adapters/child_list_adapter.cpp
	models/adapters/child_list_adapter.hpp
	models/adapters/column_list_adapter.cpp
	models/adapters/column_list_adapter.hpp
	models/adapters/indexed_adapter.hpp
	models/adapters/sequence_list_adapter.cpp
	models/adapters/sequence_list_adapter.hpp
) 
SOURCE_GROUP( "DataModel\\Adapters" FILES ${DATA_MODEL_ADAPTERS_SRCS} )

SET( DATA_MODEL_EXTENSIONS_SRCS
	models/extensions/column_extension.cpp
	models/extensions/column_extension.hpp
	models/extensions/component_extension.cpp
	models/extensions/component_extension.hpp
	models/extensions/selection_extension.cpp
	models/extensions/selection_extension.hpp
	models/extensions/tree_extension.cpp
	models/extensions/tree_extension.hpp
	models/extensions/value_extension.cpp
	models/extensions/value_extension.hpp
	models/extensions/thumbnail_extension.cpp
	models/extensions/thumbnail_extension.hpp
	models/extensions/i_model_extension.hpp
) 
SOURCE_GROUP( "DataModel\\Extensions" FILES ${DATA_MODEL_EXTENSIONS_SRCS} )

SET( HELPERS_SRCS
	helpers/asset_browser_list_filter.hpp
	helpers/asset_browser_list_filter.cpp
	helpers/qt_helpers.hpp
	helpers/qt_helpers.cpp
	helpers/wg_list_filter.hpp
	helpers/wg_list_filter.cpp
	helpers/wg_tree_filter.hpp
	helpers/wg_tree_filter.cpp
    helpers/selection_helper.cpp
    helpers/selection_helper.hpp
	)
SOURCE_GROUP( "Helpers" FILES ${HELPERS_SRCS} )


SET( CONTROLS_SRC
	controls/bw_checkbox.hpp
	controls/bw_checkbox.cpp
	controls/bw_checkbox.qml
	controls/bw_combobox.hpp
	controls/bw_combobox.cpp
	controls/bw_combobox.qml
	controls/bw_copyable.hpp
	controls/bw_copyable.cpp
	controls/bw_filedialog_qi.hpp
	controls/bw_filedialog_qi.cpp
	controls/bw_textfield.hpp
	controls/bw_textfield.cpp
	controls/bw_textfield.qml
	controls/popup_window.hpp
	controls/popup_window.cpp
)
SOURCE_GROUP( "Controls" FILES ${CONTROLS_SRC} )

SET( INTERFACES
	interfaces/i_check_filter.hpp
)
SOURCE_GROUP( "Interface" FILES ${INTERFACES} )


SET( RESOURCE_FILES
	qt_common.qrc
)
SOURCE_GROUP( "Resource Files" FILES ${RESOURCE_FILES} )
qt5_add_resources( COMPILED_RESOURCES ${RESOURCE_FILES} )
SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )


SET( ALL_SRCS
	shared_controls.hpp
	shared_controls.cpp
	i_qt_framework.hpp
	i_qt_type_converter.hpp
	string_qt_type_converter.hpp
	vector_qt_type_converter.hpp
	qml_component.cpp
	qml_component.hpp
	qml_view.cpp
	qml_view.hpp
	qt_action_manager.cpp
	qt_action_manager.hpp
	qt_image_provider.cpp
	qt_image_provider.hpp
	qt_connection_holder.hpp
	qt_connection_holder.cpp
	qt_new_handler.hpp
	qt_new_handler.cpp
	qt_palette.hpp
	qt_palette.cpp
	qt_default_spacing.hpp
	qt_global_settings.hpp
	qt_window.cpp
	qt_window.hpp
	qml_window.cpp
	qml_window.hpp
	qt_dock_region.cpp
	qt_dock_region.hpp
	qt_tab_region.cpp
	qt_tab_region.hpp
	qt_menu.cpp
	qt_menu.hpp
	qt_menu_bar.cpp
	qt_menu_bar.hpp
	qt_tool_bar.cpp
	qt_tool_bar.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
	${CONTROLS_SRC} 
	${DATA_MODEL_SRCS}
	${DATA_MODEL_ADAPTERS_SRCS}
	${DATA_MODEL_EXTENSIONS_SRCS}
	${HELPERS_SRCS}
	${INTERFACES}
)
BW_ADD_LIBRARY( core_qt_common ${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( core_qt_common INTERFACE
	core_reflection
	core_data_model
	core_logging
	core_string_utils
	core_copy_paste
)

# Link QT5 modules
qt5_use_modules( core_qt_common LINK_PRIVATE Core Gui QuickWidgets Quick UiTools Xml )

BW_PROJECT_CATEGORY( core_qt_common "NGT Libs" )

