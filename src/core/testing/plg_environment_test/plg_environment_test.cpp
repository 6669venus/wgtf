#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_variant/variant.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_view.hpp"

#include "project/metadata/project.mpp"
#include <vector>



namespace{

    class ProjectManager
    {
    public:
        ProjectManager( IComponentContext & contextManager )
            : contextManager_( contextManager )
        {

        }
        void init()
        {
            auto uiFramework = contextManager_.queryInterface<IUIFramework>();
            auto uiApplication = contextManager_.queryInterface<IUIApplication>();
            assert( uiFramework != nullptr && uiApplication != nullptr );
            uiFramework->loadActionData( 
                ":/testing_project/actions.xml", IUIFramework::ResourceType::File );

            newProject_ = uiFramework->createAction(
                "NewProject", 
                std::bind( &ProjectManager::newProject, this ) );

            openProject_ = uiFramework->createAction(
                "OpenProject", 
                std::bind( &ProjectManager::openProject, this ),
                std::bind( &ProjectManager::canOpen, this ) );

            saveProject_ = uiFramework->createAction(
                "SaveProject", 
                std::bind( &ProjectManager::saveProject, this ),
                std::bind( &ProjectManager::canSave, this ) );

            closeProject_ = uiFramework->createAction(
                "CloseProject", 
                std::bind( &ProjectManager::closeProject, this ),
                std::bind( &ProjectManager::canClose, this ) );

            uiApplication->addAction( *newProject_ );
            uiApplication->addAction( *openProject_ );
            uiApplication->addAction( *saveProject_ );
            uiApplication->addAction( *closeProject_ );

        }
        void fini()
        {
            auto uiApplication = contextManager_.queryInterface<IUIApplication>();
            assert( uiApplication != nullptr );
            uiApplication->removeAction( *newProject_ );
            uiApplication->removeAction( *openProject_ );
            uiApplication->removeAction( *saveProject_ );
            uiApplication->removeAction( *closeProject_ );
            newProject_ = nullptr;
            openProject_ = nullptr;
            saveProject_ = nullptr;
            closeProject_ = nullptr;
        }

    private:

        void newProject()
        {

        }
        void openProject()
        {

        }
        void saveProject()
        {

        }
        void closeProject()
        {

        }
        bool canOpen()
        {
            return true;
        }

        bool canSave()
        {
            return curProject_ != nullptr;
        }

        bool canClose()
        {
            return curProject_ != nullptr;
        }


        IComponentContext& contextManager_;
        std::vector<std::string> savedProjectIds_;
        std::unordered_map<std::string, ObjectHandle > projects_;
        std::unordered_map<std::string, std::unique_ptr<IView> > projectViews_;
        ObjectHandle curProject_;
        std::unique_ptr< IAction > newProject_;
        std::unique_ptr< IAction > openProject_;
        std::unique_ptr< IAction > saveProject_;
        std::unique_ptr< IAction > closeProject_;
    };
}


//==============================================================================
class EnvrionmentTestPlugin
	: public PluginMain
{
private:
	std::vector<IInterface*> types_;
    std::unique_ptr<ProjectManager> projectManager_;
public:
	//==========================================================================
	EnvrionmentTestPlugin(IComponentContext & contextManager )
        : projectManager_( new ProjectManager( contextManager ))
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );
		// register reflected type definition
		IDefinitionManager* defManager =
			contextManager.queryInterface< IDefinitionManager >();
		assert(defManager != nullptr);

		this->initReflectedTypes( *defManager );

        projectManager_->init();

	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
        projectManager_->fini();
		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
        REGISTER_DEFINITION( Project )
	}

};


PLG_CALLBACK_FUNC( EnvrionmentTestPlugin )

