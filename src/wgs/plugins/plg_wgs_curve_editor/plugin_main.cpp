#include "generic_plugin/generic_plugin.hpp"
#include "qt_common/shared_controls.hpp"
#include "reflection/type_class_definition.hpp"
#include <QWidget>
#include <QQmlEngine>
#include <QQmlContext>

#include "generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_window.hpp"
#include "ui_framework/i_view.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "reflection/reflection_macros.hpp"
#include "metadata/curves_model.mpp"

class CurveDataModel
{

};

class CurveEditorPlugin
	: public PluginMain
{
public:
	CurveEditorPlugin(IContextManager & contextManager) : curvesModel_(nullptr) {}

	bool PostLoad( IContextManager & contextManager ) override
	{
		Q_INIT_RESOURCE(qml);
		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
		auto metaTypeMgr = contextManager.queryInterface< IMetaTypeManager >();
		assert(metaTypeMgr);
		Variant::setMetaTypeManager(metaTypeMgr);

		SharedControls::init();
		// Setup the model(s) for the view
		auto& definitionManager = *contextManager.queryInterface<IDefinitionManager>();
		assert(&definitionManager != nullptr);

		REGISTER_DEFINITION(Point);
		REGISTER_DEFINITION(BezierPoint);
		REGISTER_DEFINITION(Curve);
		REGISTER_DEFINITION(CurvesModel);

		auto definition = definitionManager.getDefinition(getClassIdentifier< CurvesModel >());
		assert(definition != nullptr);
		curvesModel_ = definition->create();
		auto& curves = *curvesModel_.getBase<CurvesModel>();
		
		std::vector<BezierPoint> curve1;
		curve1.emplace_back(BezierPoint(0.0f, 0.0f, -0.2f, 0.0f, 0.2f, 0.0f));
		curve1.emplace_back(BezierPoint(0.5f, 0.5f, -0.1f, -0.2f, 0.1f, 0.2f));
		curve1.emplace_back(BezierPoint(1.0f, 1.0f, -0.2f, 0.0f, 0.2f, 0.0f));
		curves.append(Curve(curve1));
		std::vector<BezierPoint> curve2;
		curve2.emplace_back(BezierPoint(0.0f, 0.0f, -0.1f, 0.0f, 0.1f, 0.1f));
		curve2.emplace_back(BezierPoint(1.0f, 0.5f, -0.1f, -0.1f, 0.1f, 0.1f));
		curves.append(Curve(curve2));
		std::vector<BezierPoint> curve3;
		curve3.emplace_back(BezierPoint(0.0f, 0.0f, -0.1f, 0.0f, 0.1f, 0.1f));
		curve3.emplace_back(BezierPoint(0.8f, 0.1f, -0.1f, -0.1f, 0.1f, 0.1f));
		curve3.emplace_back(BezierPoint(0.9f, 0.9f, -0.1f, -0.1f, 0.1f, 0.1f));
		curve3.emplace_back(BezierPoint(1.0f, 0.1f, -0.1f, -0.1f, 0.1f, 0.1f));
		curves.append(Curve(curve3));
		std::vector<BezierPoint> curve4;
		curve4.emplace_back(BezierPoint(0.0f, 0.75f, 0.0f, 0.0f, 0.1f, 0.1f));
		curve4.emplace_back(BezierPoint(1.0f, 0.25f, -0.1f, -0.1f, 0.0f, 0.0f));
		curves.append(Curve(curve4));

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		
		curvePanel_ = uiFramework->createView("qrc:///resources/main.qml", IUIFramework::ResourceType::Url, curvesModel_);
		uiApplication->addView(*curvePanel_);
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
		Q_CLEANUP_RESOURCE(qml);
	}

private:
	std::vector< IInterface * > types_;
	std::unique_ptr< IView > curvePanel_;
	ObjectHandle curvesModel_;
};

PLG_CALLBACK_FUNC(CurveEditorPlugin)

