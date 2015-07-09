INCLUDE( BWMayaProject )


SET( TARGET_PLUGIN_APP_FOLDER_NAME generic_app_test )


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
