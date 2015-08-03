INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/lib )

IF( BW_UNIT_TESTS_ENABLED )
	LIST( APPEND BW_TOOLS_UNIT_TEST_LIBRARIES
		reflection_test_objects		core/lib/reflection_objects_test
		core_unit_test		core/lib/core_unit_test
		CppUnitLite2		core/third_party/CppUnitLite2
	)

	LIST( APPEND BW_TOOLS_UNIT_TEST_BINARIES
		gps_unit_test 						core/lib/core_generic_plugin_manager/unit_test
		variant_unit_test 					core/lib/core_variant/unit_test
		command_system_unit_test 			core/lib/core_command_system/unit_test
		serialization_unit_test 			core/lib/core_serialization/unit_test
		core_common_unit_test 			    core/lib/core_common/unit_test
		reflection_unit_test 				core/lib/core_reflection/unit_test

		)

	LIST( APPEND BW_TOOLS_UNIT_TEST_PLUGINS
		plg_plugin1_test 					core/lib/core_generic_plugin_manager/unit_test/plugin1_test
		plg_plugin2_test 					core/lib/core_generic_plugin_manager/unit_test/plugin2_test
		)

	MESSAGE( STATUS "Unit tests enabled for tools." )
	ENABLE_TESTING()
ENDIF()

LIST( APPEND BW_LIBRARY_PROJECTS
	#WG Systems
	wg_types			core/lib/wg_types
	wg_memory			core/lib/wg_memory

	#NGT Systems
	core_common		        core/lib/core_common
	core_variant			core/lib/core_variant
	core_dependency_system	core/lib/core_dependency_system
	core_command_system		core/lib/core_command_system
	core_control_system		core/lib/core_control_system
	core_reflection			core/lib/core_reflection
	core_reflection_utils	core/lib/core_reflection_utils
	core_serialization		core/lib/core_serialization
	core_string_utils		core/lib/core_string_utils
	core_copy_paste			core/lib/core_copy_paste

	#Tools Common
	core_logging				core/lib/core_logging
	core_logging_system		    core/lib/core_logging_system
	core_generic_plugin		    core/lib/core_generic_plugin
	core_generic_plugin_manager	core/lib/core_generic_plugin_manager
	core_qt_common			    core/lib/core_qt_common
	core_qt_script			    core/lib/core_qt_script
	core_data_model			    core/lib/core_data_model
	core_ui_framework		    core/lib/core_ui_framework

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
	plg_variant				    core/plugins/plg_variant
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
	
	interfaces_test			    core/testing/interfaces_test
	plg_main_test				core/testing/plg_main_test
	plg_window_test				core/testing/plg_window_test
	plg_2_test					core/testing/plg_2_test
	plg_3_test					core/testing/plg_3_test
	plg_4_test					core/testing/plg_4_test
	plg_reflection_test			core/testing/plg_reflection_test
	plg_progress_manager_test	core/testing/plg_progress_manager_test
	plg_ui_main_test			core/testing/plg_ui_main_test
	plg_data_model_test			core/testing/plg_data_model_test
	plg_obj_handle_test			core/testing/plg_obj_handle_test
	plg_asset_browser_test		core/testing/plg_asset_browser_test
	
	# Unit test plugins
	${BW_TOOLS_UNIT_TEST_PLUGINS}
)

