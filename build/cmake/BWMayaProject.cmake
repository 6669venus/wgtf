SET( BW_SINGLETON_MANAGER_SUPPORT ON )
SET( NGT_SUPPORT ON )

ADD_DEFINITIONS( -DEDITOR_ENABLED )
ADD_DEFINITIONS( -DALLOW_STACK_CONTAINER )

# Hack to get Qt binary copy workign while building via this configuration file.
SET(BW_IS_QT_TOOLS ON)
SET(BW_IS_GENERIC_APP_TEST ON)

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw/lib )

SET( TARGET_PLUGIN_APP_FOLDER_NAME wgs_apps )

SET( BW_LIBRARY_PROJECTS	

	#WG Systems
	wg_types			core/lib/wg_types 
	wg_memory			core/lib/wg_memory

	#NGT Systems
	variant				core/lib/variant
	dependency_system	core/lib/dependency_system
	command_system		core/lib/command_system
	control_system		core/lib/control_system
	reflection			core/lib/reflection
	reflection_utils	core/lib/reflection_utils
	serialization		core/lib/serialization
	string_utils		core/lib/string_utils
	logging_system		core/lib/logging_system
	ngt_core_common		core/lib/ngt_core_common
	copy_paste			core/lib/copy_paste_system

	#Tools Common
	logging				core/lib/logging
	generic_plugin		core/lib/generic_plugin
	generic_plugin_manager	core/lib/generic_plugin_manager
	qt_common			core/lib/qt_common
	qt_script			core/lib/qt_script
	data_model			core/lib/data_model
	ui_framework		core/lib/ui_framework

)

SET( BW_BINARY_PROJECTS	
	# Apps
	generic_app			core/app/generic_app
	maya_plugin			core/app/maya_plugin
	
)

SET( BW_PLUGIN_PROJECTS
	# Plugins
	#plg_automation				core/plugins/plg_automation
	plg_reflection				core/plugins/plg_reflection
	plg_command_system			core/plugins/plg_command_system
	plg_type_system				core/plugins/plg_type_system
	plg_editor_interaction		core/plugins/plg_editor_interaction
	plg_control_system			core/plugins/plg_control_system
	plg_history_ui				core/plugins/plg_history_ui
	plg_macros_ui				core/plugins/plg_macros_ui
	plg_qt_app					core/plugins/plg_qt_app
	plg_qt_common				core/plugins/plg_qt_common
	plg_serialization			core/plugins/plg_serialization
	plg_progress_manager		core/plugins/plg_progress_manager
	plg_idedebug_logger			core/plugins/plg_idedebug_logger
	plg_logging_system			core/plugins/plg_logging_system
	plg_alert_ui				core/plugins/plg_alert_ui
	plg_file_system				core/plugins/plg_file_system
	plg_asset_browser			core/plugins/plg_asset_browser
	plg_copy_paste				core/plugins/plg_copy_paste
)

