INCLUDE( BWMayaProject )


SET( TARGET_PLUGIN_APP_FOLDER_NAME generic_app_test )


SET( BW_LIBRARY_PROJECTS	

	#WG Systems
	wg_types			core/lib/wg_types 
	wg_memory			core/lib/wg_memory

	#NGT Systems
	core_variant		    core/lib/core_variant
	core_common		        core/lib/core_common
	core_dependency_system	core/lib/core_dependency_system
	core_ui_framework       core/lib/core_ui_framework

	#Tools Common
	core_logging				core/lib/core_logging
	core_generic_plugin		    core/lib/core_generic_plugin
	core_generic_plugin_manager	core/lib/core_generic_plugin_manager
)

SET( BW_BINARY_PROJECTS	
	# Apps
	maya_plugin			core/app/maya_plugin
)
