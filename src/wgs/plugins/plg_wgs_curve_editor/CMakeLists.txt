CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_wgs_curve_editor )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

SET(RESOURCE_FILES
	qml.qrc
)
SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES})

SET(QML_FILES
    resources/main.qml
    resources/Point.qml
    resources/Curve.qml
    resources/ViewTransform.qml
)
SOURCE_GROUP("Resource Files\\Qml" FILES ${QML_FILES})
SET(IMAGE_FILES
    resources/images/bg_tab_selected.png
    resources/images/icon_arrow_down.png
    resources/images/icon_arrow_forward.png
    resources/images/icon_arrow_up.png
    resources/images/icon_clear_text.png
    resources/images/icon_launch_game.png
    resources/images/icon_magnifying_glass.png
    resources/images/icon_settings_dropdown.png
    resources/images/icon_settings_gear.png
    resources/images/icon_settings_gear_small.png
    resources/images/icon_status_pass.png
    resources/images/icon_update_pc.png
    resources/images/logo_WG.png
    resources/images/x.png
    resources/images/y.png
    resources/images/z.png
)
SOURCE_GROUP("Resource Files\\Images" FILES ${IMAGE_FILES})

QT5_ADD_RESOURCES(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET(MODEL_SRCS
	models/curves_model.cpp
	models/curves_model.h
)
SOURCE_GROUP("Models" FILES ${MODEL_SRCS})

SET(METADATA_SRCS
	metadata/curves_model.mpp
)
SOURCE_GROUP("Metadata" FILES ${METADATA_SRCS})

SET( ALL_SRCS
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_ADD_TOOL_PLUGIN( plg_wgs_curve_editor ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED	
	${ALL_SRCS}
	${MODEL_SRCS}
	${METADATA_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${QML_FILES}
	${IMAGE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_wgs_curve_editor PRIVATE
	generic_plugin
	qt_common
	qt_script
	ui_framework
)

# Link QT5 modules
QT5_USE_MODULES( plg_wgs_curve_editor Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_wgs_curve_editor "Plugins" )

