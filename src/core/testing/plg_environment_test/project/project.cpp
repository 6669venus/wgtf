#include "project.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_serialization/i_file_system.hpp"


namespace {
    static const char * s_projectVersion = "v0.";
    static const char * s_projectFileExtension = "ngtprj";
    static const char * s_projectDataExtension = "ngtprj_data";
    static const char * s_projectPreferenceExtension = "ngtprj_setting";

    std::string genProjectFileName( const char * projectName )
    {
        std::string s = projectName;
        s += s_projectVersion;
        s += s_projectFileExtension;
        return s;
    }

    std::string genProjectDataFileName( const char * projectName )
    {
        std::string s = projectName;
        s += s_projectVersion;
        s += s_projectDataExtension;
        return s;
    }

    std::string genProjectSettingFileName( const char * projectName )
    {
        std::string s = projectName;
        s += s_projectVersion;
        s += s_projectPreferenceExtension;
        return s;
    }
}

//////////////////////////////////////////////////////////////////////////
Project::Project()
{

}

Project::~Project()
{

}

void Project::init( IComponentContext& contextManager, const char * projectName, const char * dataFile )
{
    projectName_ = projectName;
    auto defManager = contextManager.queryInterface<IDefinitionManager>();
    auto controller = contextManager.queryInterface<IReflectionController>();
    auto fileSystem = contextManager.queryInterface<IFileSystem>();
    assert( defManager != nullptr && controller != nullptr && fileSystem != nullptr );
    if( dataFile == nullptr || !fileSystem->exists(dataFile))
    {
        auto pDefinition = defManager->getDefinition(
            getClassIdentifier< ProjectData >() );
        assert( pDefinition != nullptr );
        projectData_ = pDefinition->create();
    }
    else
    {
        IFileSystem::IStreamPtr fileStream = 
            fileSystem->readFile( dataFile, std::ios::in | std::ios::binary );
        XMLSerializer serializer( *fileStream, *defManager );
        Variant variant;
        bool br = serializer.deserialize( variant );
        assert( br );
        br = variant.tryCast( projectData_ );
        assert( br );
    }
    
    auto model = std::unique_ptr< ITreeModel >(
        new ReflectedTreeModel( projectData_, *defManager, controller ) );

    auto uiFramework = contextManager.queryInterface<IUIFramework>();
    auto uiApplication = contextManager.queryInterface<IUIApplication>();
    assert( uiFramework != nullptr && uiApplication != nullptr );
    view_ = uiFramework->createView( "testing_project/project_data_panel.qml", 
        IUIFramework::ResourceType::Url, std::move( model ) );
    uiApplication->addView( *view_ );
}

void Project::fini( IComponentContext& contextManager )
{
    auto uiApplication = contextManager.queryInterface<IUIApplication>();
    assert( uiApplication != nullptr );
    uiApplication->removeView( *view_ );
    view_ = nullptr;
    projectData_ = nullptr;
}

void Project::saveData( IComponentContext& contextManager, const char * dataFile )
{
    auto defManager = contextManager.queryInterface<IDefinitionManager>();
    auto fileSystem = contextManager.queryInterface<IFileSystem>();
    assert( defManager && fileSystem );
    ResizingMemoryStream stream;
    XMLSerializer serializer( stream, *defManager );
    defManager->serializeDefinitions( serializer );
    serializer.serialize( projectData_ );
    serializer.sync();
    fileSystem->writeFile( 
        dataFile, stream.buffer().c_str(), 
        stream.buffer().size(), std::ios::out | std::ios::binary );
}

const char * Project::getProjectName() const
{
    return projectName_.c_str();
}

const ObjectHandle & Project::getProjectData() const
{
    return projectData_;
}

 //////////////////////////////////////////////////////////////////////////
ProjectManager::ProjectManager()
{
}
void ProjectManager::init( IComponentContext& contextManager )
{
    contextManager_ = &contextManager;
}
void ProjectManager::fini()
{
}

void ProjectManager::createProject()
{
    this->closeProject();
    curProject_.reset( new Project );
    curProject_->init( *contextManager_, newProjectName_.c_str() );
}
void ProjectManager::openProject( const Variant& strProjectFile )
{
    auto defManager = contextManager_->queryInterface<IDefinitionManager>();
    auto fileSystem = contextManager_->queryInterface<IFileSystem>();
    auto uiFramework = contextManager_->queryInterface<IUIFramework>();
    assert( defManager && fileSystem && uiFramework );
    std::string projectFile;
    bool isOk = strProjectFile.tryCast( projectFile );
    assert( isOk );
    IFileSystem::IStreamPtr fileStream = 
        fileSystem->readFile( projectFile.c_str(), std::ios::in | std::ios::binary );
    XMLSerializer serializer( *fileStream, *defManager );
    std::string projectName;
    std::string projectDataFile;
    std::string projectSettingFile;
    serializer.deserialize( projectName );
    serializer.deserialize( projectDataFile );
    serializer.deserialize( projectSettingFile );

    // load preference
    //uiFramework->getPreferences()->loadPreferenceFromFile( projectSettingFile.c_str() );

    // load data
    this->closeProject();
    curProject_.reset( new Project );
    curProject_->init( *contextManager_, newProjectName_.c_str(), projectDataFile.c_str() );
}
void ProjectManager::saveProject()
{
    
    auto defManager = contextManager_->queryInterface<IDefinitionManager>();
    auto fileSystem = contextManager_->queryInterface<IFileSystem>();
    auto uiFramework = contextManager_->queryInterface<IUIFramework>();
    if (uiFramework && defManager && fileSystem)
    {
        std::string projectFile = genProjectFileName( curProject_->getProjectName() );
        std::string projectDataFile = genProjectDataFileName( curProject_->getProjectName() );
        std::string projectSettingFile = genProjectSettingFileName( curProject_->getProjectName() );

        //save project data
        curProject_->saveData( *contextManager_, projectDataFile.c_str() );

        //save project UI preferences
        uiFramework->getPreferences()->savePreferenceToFile(projectSettingFile.c_str());

        //save project itself
        ResizingMemoryStream stream;
        XMLSerializer serializer( stream, *defManager );
        serializer.serialize( curProject_->getProjectName() );
        serializer.serialize( projectDataFile );
        serializer.serialize( projectSettingFile );
        serializer.sync();
        fileSystem->writeFile( 
            projectFile.c_str(), stream.buffer().c_str(), stream.buffer().size(), std::ios::out | std::ios::binary );
    }
    else
    {
        assert( false );
    }
    
}
void ProjectManager::closeProject()
{
    if(curProject_ != nullptr)
    {
        curProject_->fini( *contextManager_ );
        curProject_ = nullptr;
    }
}
bool ProjectManager::canOpen()
{
    return true;
}

bool ProjectManager::canSave()
{
    return curProject_ != nullptr;
}

bool ProjectManager::canClose()
{
    return curProject_ != nullptr;
}

bool ProjectManager::isProjectNameOk( const Variant& strProjectName )
{
    assert( strProjectName.canCast<std::string>());
    strProjectName.tryCast( newProjectName_ );
    return true;
}

//////////////////////////////////////////////////////////////////////////
ProjectData::ProjectData()
	: bChecked_( false)
	, text_( L"Hello Test" )
	, curSlideData_( 0 )
	, curNum_( 100 )
	, curSelected_( 0 )
	, enumValue_( 0 )
	, vec3_(0.0f, 0.0f, 0.0f)
	, vec4_(0.0f, 0.0f, 0.0f, 0.0f)
	, color3_(192.0f, 192.0f, 192.0f)
	, color4_(127.0f, 127.0f, 127.0f, 127.0f)
{
}

ProjectData::~ProjectData()
{
}

void ProjectData::setCheckBoxState( const bool & bChecked )
{
	bChecked_ = bChecked;
}
void ProjectData::getCheckBoxState( bool * bChecked ) const
{
	*bChecked = bChecked_;
}

void ProjectData::setTextField( const std::wstring & text )
{
	text_ = text;
}
void ProjectData::getTextField( std::wstring * text ) const
{
	*text = text_;
}

void ProjectData::setSlideData( const double & length )
{
	if ((length < this->getSlideMinData()) || (length > this->getSlideMaxData()))
	{
		return;
	}
	curSlideData_ = length;
}
void ProjectData::getSlideData(double * length) const
{
	*length = curSlideData_;
}

int ProjectData::getSlideMaxData()
{
	return 100;
}
int ProjectData::getSlideMinData()
{
	return -100;
}

void ProjectData::setNumber( const int & num )
{
	curNum_ = num;
}
void ProjectData::getNumber( int * num ) const
{
	*num = curNum_;
}

void ProjectData::setSelected( const int & select )
{
	curSelected_ = select;
}
void ProjectData::getSelected( int * select ) const
{
	*select = curSelected_;
}

void ProjectData::setVector3(const Vector3 & vec3)
{
	vec3_.x = vec3.x;
	vec3_.y = vec3.y;
	vec3_.z = vec3.z;
}
void ProjectData::getVector3(Vector3 * vec3) const
{
	vec3->x = vec3_.x;
	vec3->y = vec3_.y;
	vec3->z = vec3_.z;
}

void ProjectData::setVector4(const Vector4 & vec4)
{
	vec4_.x = vec4.x;
	vec4_.y = vec4.y;
	vec4_.z = vec4.z;
	vec4_.w = vec4.w;
}
void ProjectData::getVector4(Vector4 * vec4) const
{
	vec4->x = vec4_.x;
	vec4->y = vec4_.y;
	vec4->z = vec4_.z;
	vec4->w = vec4_.w;
}

void ProjectData::setColor3(const Vector3 & color)
{
	color3_.x = color.x;
	color3_.y = color.y;
	color3_.z = color.z;
}
void ProjectData::getColor3(Vector3 * color) const
{
	color->x = color3_.x;
	color->y = color3_.y;
	color->z = color3_.z;
}

void ProjectData::setColor4(const Vector4 & color)
{
	color4_.x = color.x;
	color4_.y = color.y;
	color4_.z = color.z;
	color4_.w = color.w;
}
void ProjectData::getColor4(Vector4 * color) const
{
	color->x = color4_.x;
	color->y = color4_.y;
	color->z = color4_.z;
	color->w = color4_.w;
}

