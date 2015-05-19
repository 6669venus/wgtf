#include "generic_plugin_system/generic_plugin.hpp"
#include "cstdmf/string_utils.hpp"
#include "common/string_utils.hpp"
#include "cstdmf/bw_string.hpp"
#include "resmgr/bwresource.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "serialization/interfaces/i_file_utilities.hpp"
#include "serializer/bw_type_serializer.hpp"
#include "variant/variant.hpp"
#include "qt_type_converters.hpp"
#include "data_model/i_item_role.hpp"

#include "qt_common/i_qt_framework.hpp"
#include "ui_framework/generic_component_provider.hpp"

#include "wg_types/string_ref.hpp"

class BWFileUtilities
	: public Implements< IFileUtilities >
{
	StringRef getFilename( const StringRef & file )
	{
		auto result = BW::BWResource::getFilename(
			BW::StringRef( file.data(), file.length() ) );
		return StringRef( result.data(), result.length() );
	}


	StringRef getExtension( const StringRef & file )
	{
		auto result = BW::BWResource::getExtension(
			BW::StringRef( file.data(), file.length() ) );
		return StringRef( result.data(), result.length() );
	}


	StringRef removeExtension( const StringRef & file )
	{
		auto result = BW::BWResource::removeExtension(
			BW::StringRef( file.data(), file.length() ) );
		return StringRef( result.data(), result.length() );
	}


	bool fileExists( const StringRef & file ) override
	{
		return BW::BWResource::fileExists(
			BW::StringRef( file.data(), file.length() ) );
	}

	std::string resolveFilename( const StringRef & file ) override
	{
		auto result = BW::BWResolver::resolveFilename(
			BW::StringRef( file.data(), file.length() ) );
		return std::string( result.data(), result.length() );
	}
};

//==============================================================================
class BWCorePlugin
	: public PluginMain
{
public:
	//==========================================================================
	BWCorePlugin( IContextManager & contextManager )
		: bwTypeSerializer_( new BWTypeSerializer )
		, bwVector2MetaType_( new MetaTypeImpl<BW::Vector2> )
		, bwVector3MetaType_( new MetaTypeImpl<BW::Vector3> )
		, bwVector4MetaType_( new MetaTypeImpl<BW::Vector4> )
		, bw_FileUtilities_( new BWFileUtilities )
	{ 
		new BW::BWResource();
		// init BWResource
		wchar_t * cmdLine = ::GetCommandLine();
		if(!parseCommandLineMF( cmdLine ) )
		{
			assert( false );
		}
		contextManager.registerInterface( bw_FileUtilities_.get(), false );
	}

	void Initialise( IContextManager & contextManager )
	{
		auto metaTypeMgr = contextManager.queryInterface<IMetaTypeManager>();
		if(metaTypeMgr)
		{
			Variant::setMetaTypeManager( metaTypeMgr );
			metaTypeMgr->registerType( bwVector2MetaType_.get() );
			metaTypeMgr->registerType( bwVector3MetaType_.get() );
			metaTypeMgr->registerType( bwVector4MetaType_.get() );
		}
		// create and register a serializer
		auto serializationManager = 
			contextManager.queryInterface<ISerializationManager>();
		if (serializationManager)
		{
			for(auto type : bwTypeSerializer_->getSupportedType())
			{
				serializationManager->registerSerializer( 
					type.getName(), bwTypeSerializer_.get() );
			}
		}

		auto qtFramework = contextManager.queryInterface< IQtFramework >();
		if (qtFramework)
		{
			qtTypeConverters_.emplace_back( new BWStringQtTypeConverter() );
			qtTypeConverters_.emplace_back( new VectorQtTypeConverter() );

			for (auto & qtTypeConverter : qtTypeConverters_)
			{
				qtFramework->registerTypeConverter( *qtTypeConverter );
			}
		}

		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		if (uiFramework)
		{
			componentProviders_.emplace_back(
				new GenericComponentProvider<BW::string>( "string" ) );
			componentProviders_.emplace_back(
				new GenericComponentProvider<BW::wstring>( "string" ) );
			componentProviders_.emplace_back(
				new GenericComponentProvider<BW::Vector3>( "vector3" ) );
			componentProviders_.emplace_back(
				new GenericComponentProvider<BW::Vector4>( "vector4" ) );
			size_t colorRoles[] = { IsColorRole::roleId_ };
			componentProviders_.emplace_back(
				new GenericComponentProvider<BW::Vector3>( "color3", colorRoles ) );
			componentProviders_.emplace_back(
				new GenericComponentProvider<BW::Vector4>( "color4", colorRoles ) );

			for (auto & componentProvider : componentProviders_)
			{
				uiFramework->registerComponentProvider( *componentProvider );
			}
		}
	}

	//==========================================================================
	void Unload( IContextManager & contextManager ) override
	{
		auto serializationManager = 
			contextManager.queryInterface<ISerializationManager>();
		if (serializationManager)
		{
			for(auto type : bwTypeSerializer_->getSupportedType())
			{
				serializationManager->deregisterSerializer( type.getName() );
			}
			bwTypeSerializer_ = nullptr;
		}
		delete BW::BWResource::pInstance();
		BW::BWResource::fini();
	}

private:
	bool parseCommandLineMF( wchar_t * commandLine )
	{
		// parse command line
		const int MAX_ARGS = 20;
		char * argv[ MAX_ARGS ];
		int argc = 0;

		char cmdline [32768];
		BW::bw_wtoutf8( commandLine, wcslen( commandLine ), 
			cmdline, sizeof(cmdline) / sizeof(cmdline[0]) );
		char * str = cmdline;
		while (char * token = BW::StringUtils::retrieveCmdTokenT( str ))
		{
			if (argc >= MAX_ARGS)
			{
				assert( !"plg_bw_core:parseCommandLineMF: Too many arguments!!\n" );
				return false;
			}
			argv[argc++] = token;
		}

		bool result = true;
		result &= BW::BWResource::init( argc, const_cast< const char** >( argv ) );
		return result;
	}

	std::unique_ptr<BWTypeSerializer> bwTypeSerializer_;
	std::unique_ptr< MetaTypeImpl<BW::Vector2> > bwVector2MetaType_;
	std::unique_ptr< MetaTypeImpl<BW::Vector3> > bwVector3MetaType_;
	std::unique_ptr< MetaTypeImpl<BW::Vector4> > bwVector4MetaType_;
	std::unique_ptr< BWFileUtilities > bw_FileUtilities_;
	std::vector< std::unique_ptr< IQtTypeConverter > > qtTypeConverters_;
	std::vector< std::unique_ptr< IComponentProvider > > componentProviders_;
};

PLG_CALLBACK_FUNC( BWCorePlugin )

#include "cstdmf/watcher.hpp"
#include "cstdmf/stack_tracker.hpp"
namespace BW
{
	DECLARE_WATCHER_DATA( "BW::ModelEditorTabsPlugin" )
	DECLARE_COPY_STACK_INFO( true )
}
