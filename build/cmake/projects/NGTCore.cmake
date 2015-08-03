INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/lib )

IF( BW_UNIT_TESTS_ENABLED )
	LIST( APPEND BW_TOOLS_UNIT_TEST_LIBRARIES
		reflection_test_objects		core/lib/reflection_objects_test
		unit_test_lib		core/lib/core_unit_test
		CppUnitLite2		core/third_party/CppUnitLite2
	)

	LIST( APPEND BW_TOOLS_UNIT_TEST_BINARIES
		gps_unit_test 						core/lib/core_generic_plugin_manager/unit_test
		variant_unit_test 					core/lib/core_variant/unit_test
		command_system_unit_test 			core/lib/core_command_system/unit_test
		serialization_unit_test 			core/lib/core_serialization/unit_test
		ngt_core_common_unit_test 			core/lib/core_ngt_common/unit_test
		reflection_unit_test 				core/lib/core_reflection/unit_test

		)

	LIST( APPEND BW_TOOLS_UNIT_TEST_PLUGINS
		gps_unit_test_plg1 					core/lib/core_generic_plugin_manager/unit_test/plugin1_test
		gps_unit_test_plg2 					core/lib/core_generic_plugin_manager/unit_test/plugin2_test
		)

	MESSAGE( STATUS "Unit tests enabled for tools." )
	ENABLE_TESTING()
ENDIF()

LIST( APPEND BW_LIBRARY_PROJECTS
	#WG Systems
	wg_types			core/lib/wg_types
	wg_memory			core/lib/wg_memory

	#NGT Systems
	ngt_core_common		core/lib/core_ngt_common
	variant				core/lib/core_variant
	dependency_system	core/lib/core_dependency_system
	command_system		core/lib/core_command_system
	control_system		core/lib/core_control_system
	reflection			core/lib/core_reflection
	reflection_utils	core/lib/core_reflection_utils
	serialization		core/lib/core_serialization
	string_utils		core/lib/core_string_utils
	copy_paste			core/lib/core_copy_paste

	#Tools Common
	logging				core/lib/core_logging
	logging_system		core/lib/core_logging_system
	generic_plugin		core/lib/core_generic_plugin
	generic_plugin_manager	core/lib/core_generic_plugin_manager
	qt_common			core/lib/core_qt_common
	qt_script			core/lib/core_qt_script
	data_model			core/lib/core_data_model
	ui_framework		core/lib/core_ui_framework

	# Unit test libs
	${BW_TOOLS_UNIT_TEST_LIBRARIES}
)

LIST( APPEND BW_BINARY_PROJECTS
	# Apps
	generic_app			core/app/generic_app

	# Unit tests
	${BW_TOOLS_UNIT_TEST_BINARIES}
)

LIST( APPEND BW_PLUGIN_PROJECTS
	# Plugins
	plg_automation				core/plugins/plg_automation
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
	plg_logging_system			core/plugins/plg_logging_system
	plg_idedebug_logger			core/plugins/plg_idedebug_logger
	plg_alert_ui				core/plugins/plg_alert_ui
	plg_file_system				core/plugins/plg_file_system

	plg_copy_paste				core/plugins/plg_copy_paste
	
	plg_test_interfaces			core/testing/interfaces_test
	plg_test_main				core/testing/plg_main_test
	plg_test_window				core/testing/plg_window_test
	plg_test_2					core/testing/plg_2_test
	plg_test_3					core/testing/plg_3_test
	plg_test_4					core/testing/plg_4_test
	plg_test_reflection			core/testing/plg_reflection_test
	plg_test_progress_manager	core/testing/plg_progress_manager_test
	plg_test_ui_main			core/testing/plg_ui_main_test
	plg_test_data_model			core/testing/plg_data_model_test
	plg_test_obj_handle			core/testing/plg_obj_handle_test
	plg_test_asset_browser		core/testing/plg_asset_browser_test
	
	# Unit test plugins
	${BW_TOOLS_UNIT_TEST_PLUGINS}
)

