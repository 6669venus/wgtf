#include "project.hpp"
#include "core_generic_plugin\interfaces\i_component_context.hpp"


//////////////////////////////////////////////////////////////////////////
Project::Project()
{

}

Project::~Project()
{

}

void Project::setProjectName( const char * projectName )
{
    projectName_ = projectName;
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
}
void ProjectManager::openProject( const Variant& strProjectName )
{
}
void ProjectManager::saveProject()
{
}
void ProjectManager::closeProject()
{
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

