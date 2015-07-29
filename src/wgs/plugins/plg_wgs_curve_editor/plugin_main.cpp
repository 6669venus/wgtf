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
	CurveEditorPlugin(IComponentContext & contextManager) : curvesModel_(nullptr) {}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		Q_INIT_RESOURCE(qml);
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		auto metaTypeMgr = contextManager.queryInterface< IMetaTypeManager >();
		assert(metaTypeMgr);
		if (metaTypeMgr == nullptr)
			return;
		Variant::setMetaTypeManager(metaTypeMgr);

		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		assert(definitionManager != nullptr);
		if (definitionManager == nullptr)
			return;

		SharedControls::init();

		// Setup the model(s) for the view
		typedef TypeClassDefinition<Point> PointDef;
		typedef TypeClassDefinition<BezierPoint> BezierPointDef;
		typedef TypeClassDefinition<Curve> CurveDef;
		auto pointDef = definitionManager->registerDefinition(new PointDef);
		auto bezPtDef = definitionManager->registerDefinition(new BezierPointDef);
		auto curveDef = definitionManager->registerDefinition(new CurveDef);
		auto modelDef = definitionManager->registerDefinition(new TypeClassDefinition<CurvesModel>);

		curvesModel_ = definitionManager->create< CurvesModel >();
		assert(curvesModel_ != nullptr);

		auto& curves = *curvesModel_.getBase<CurvesModel>();
		
		std::vector<ObjectHandle> curve1;
		curve1.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.0f, 0.0f),
			PointDef::create(*pointDef, -0.2f, 0.0f),
			PointDef::create(*pointDef, 0.2f, 0.0f)));
		curve1.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.0f, 0.0f),
			PointDef::create(*pointDef, -0.2f, 0.0f),
			PointDef::create(*pointDef, 0.2f, 0.0f)));
		curve1.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.5f, 0.5f),
			PointDef::create(*pointDef, -0.1f, -0.2f),
			PointDef::create(*pointDef, 0.1f, 0.2f)));
		curve1.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 1.0f, 1.0f),
			PointDef::create(*pointDef, -0.2f, 0.0f),
			PointDef::create(*pointDef, 0.2f, 0.0f)));
		curves.append(CurveDef::create(*curveDef, definitionManager, curve1));
		std::vector<ObjectHandle> curve2;
		curve2.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.0f, 0.0f),
			PointDef::create(*pointDef, -0.1f, 0.0f),
			PointDef::create(*pointDef, 0.1f, 0.1f)));
		curve2.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 1.0f, 0.5f),
			PointDef::create(*pointDef, -0.1f, -0.1f),
			PointDef::create(*pointDef, 0.1f, 0.1f)));
		curves.append(CurveDef::create(*curveDef, definitionManager, curve2));
		std::vector<ObjectHandle> curve3;
		curve3.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.0f, 0.0f),
			PointDef::create(*pointDef, -0.1f, 0.0f),
			PointDef::create(*pointDef, 0.1f, 0.1f)));
		curve3.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.8f, 0.1f),
			PointDef::create(*pointDef, -0.1f, -0.1f),
			PointDef::create(*pointDef, 0.1f, 0.1f)));
		curve3.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.9f, 0.9f),
			PointDef::create(*pointDef, -0.1f, -0.1f),
			PointDef::create(*pointDef, 0.1f, 0.1f)));
		curve3.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 1.0f, 0.1f),
			PointDef::create(*pointDef, -0.1f, -0.1f),
			PointDef::create(*pointDef, 0.1f, 0.1f)));
		curves.append(CurveDef::create(*curveDef, definitionManager, curve3));
		std::vector<ObjectHandle> curve4;
		curve4.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 0.0f, 0.75f),
			PointDef::create(*pointDef, 0.0f, 0.0f),
			PointDef::create(*pointDef, 0.1f, 0.1f)));
		curve4.emplace_back(BezierPointDef::create(*bezPtDef,
			PointDef::create(*pointDef, 1.0f, 0.25f),
			PointDef::create(*pointDef, -0.1f, -0.1f),
			PointDef::create(*pointDef, 0.0f, 0.0f)));
		curves.append(CurveDef::create(*curveDef, definitionManager, curve4));

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		
		curvePanel_ = uiFramework->createView("qrc:///resources/main.qml", IUIFramework::ResourceType::Url, curvesModel_);
		uiApplication->addView(*curvePanel_);
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
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

