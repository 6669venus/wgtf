SET( BW_SINGLETON_MANAGER_SUPPORT ON )
SET( NGT_SUPPORT ON )

ADD_DEFINITIONS( -DEDITOR_ENABLED )
ADD_DEFINITIONS( -DALLOW_STACK_CONTAINER )

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw/lib )

SET( TARGET_PLUGIN_APP_FOLDER_NAME generic_app_test )

IF( BW_UNIT_TESTS_ENABLED )
	SET( BW_TOOLS_UNIT_TEST_LIBRARIES
		reflection_test_objects		core/testing/reflection_test_objects
		
		unit_test_lib		core/lib/unit_test_lib
		CppUnitLite2		core/third_party/CppUnitLite2
	)

	SET( BW_TOOLS_UNIT_TEST_BINARIES
		gps_unit_test 						core/lib/generic_plugin_manager/unit_test
		variant_unit_test 					core/lib/variant/unit_test
		command_system_unit_test 			core/lib/command_system/unit_test
		serialization_unit_test 			core/lib/serialization/unit_test
		ngt_core_common_unit_test 			core/lib/ngt_core_common/unit_test
		reflection_unit_test 				core/testing/reflection_unit_test

		editor_shared_unit_test 			bw/lib/editor_shared/unit_test
		)

	SET( BW_TOOLS_UNIT_TEST_PLUGINS
		gps_unit_test_plg1 					core/lib/generic_plugin_manager/unit_test/test_plugin1
		gps_unit_test_plg2 					core/lib/generic_plugin_manager/unit_test/test_plugin2
		)

	MESSAGE( STATUS "Unit tests enabled for tools." )
	ENABLE_TESTING()
ENDIF()

SET( BW_LIBRARY_PROJECTS
	# BigWorld Adapters
	bw_adapters 		bw/adapters

	#WG Systems
	wg_types			core/lib/wg_types
	wg_memory			core/lib/wg_memory

	#NGT Systems
	ngt_core_common		core/lib/ngt_core_common
	variant				core/lib/variant
	dependency_system	core/lib/dependency_system
	command_system		core/lib/command_system
	control_system		core/lib/control_system
	reflection			core/lib/reflection
	reflection_utils	core/lib/reflection_utils
	serialization		core/lib/serialization
	string_utils		core/lib/string_utils
	copy_paste			core/lib/copy_paste_system

	#Tools Common
	logging				core/lib/logging
	logging_system		core/lib/logging_system
	generic_plugin		core/lib/generic_plugin
	generic_plugin_manager	core/lib/generic_plugin_manager
	qt_common			core/lib/qt_common
	qt_script			core/lib/qt_script
	data_model			core/lib/data_model
	ui_framework		core/lib/ui_framework

	editor_shared		bw/lib/editor_shared
	bw_qt_common		bw/lib/bw_qt_common

	# Unit test libs
	${BW_TOOLS_UNIT_TEST_LIBRARIES}
)

SET( BW_BINARY_PROJECTS
	# Apps
	generic_app			core/app/generic_app

	# Unit tests
	${BW_TOOLS_UNIT_TEST_BINARIES}
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
	plg_logging_system			core/plugins/plg_logging_system
	plg_idedebug_logger			core/plugins/plg_idedebug_logger
	plg_alert_ui				core/plugins/plg_alert_ui
	plg_file_system				core/plugins/plg_file_system
	plg_asset_browser			core/plugins/plg_asset_browser

	plg_copy_paste				core/plugins/plg_copy_paste
	
	plg_bw_core					bw/plugins/plg_bw_core

	plg_test_interfaces			core/testing/interfaces
	plg_test_main				core/testing/plg_test_main
	plg_test_window				core/testing/plg_test_window
	plg_test_2					core/testing/plg_test_2
	plg_test_3					core/testing/plg_test_3
	plg_test_4					core/testing/plg_test_4
	plg_test_reflection			core/testing/plg_test_reflection
	plg_test_progress_manager	core/testing/plg_test_progress_manager
	plg_test_ui_main			core/testing/plg_test_ui_main
	
	plg_test_bw_types			bw/testing/plg_test_bw_types

	# Unit test plugins
	${BW_TOOLS_UNIT_TEST_PLUGINS}
)

