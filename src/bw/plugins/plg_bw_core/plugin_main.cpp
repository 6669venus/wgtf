#include "generic_plugin/generic_plugin.hpp"
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

#include "ngt_core_common/ngt_windows.hpp"

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
namespace
{
	const char g_separator = ',';

	class Vector2MetaType
		: public MetaTypeImpl<BW::Vector2>
	{
		typedef MetaTypeImpl<BW::Vector2> base;

	public:
		Vector2MetaType():
			base()
		{
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			const BW::Vector2 & vec = *cast(value);
			stream << vec.x <<g_separator << vec.y;
			return stream.good();
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if (!stream.good())
			{
				return false;
			}
			BW::Vector2 & vec = *cast(value);
			char separator;
			stream >> vec.x >> separator >> vec.y;
			return !stream.fail();
		}

	private:
		static BW::Vector2* cast(void* value)
		{
			return static_cast<BW::Vector2*>(value);
		}

		static const BW::Vector2* cast(const void* value)
		{
			return static_cast<const BW::Vector2*>(value);
		}

	};

	class Vector3MetaType
		: public MetaTypeImpl<BW::Vector3>
	{
		typedef MetaTypeImpl<BW::Vector3> base;

	public:
		Vector3MetaType():
			base()
		{
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			const BW::Vector3 & vec = *cast(value);
			stream << vec.x << g_separator << vec.y << g_separator << vec.z;
			return stream.good();
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if (!stream.good())
			{
				return false;
			}
			BW::Vector3 & vec = *cast(value);
			char separator;
			stream >> vec.x >> separator >> vec.y >> separator >> vec.z;
			return !stream.fail();
		}

	private:
		static BW::Vector3* cast(void* value)
		{
			return static_cast<BW::Vector3*>(value);
		}

		static const BW::Vector3* cast(const void* value)
		{
			return static_cast<const BW::Vector3*>(value);
		}

	};

	class Vector4MetaType
		: public  MetaTypeImpl<BW::Vector4>
	{
		typedef  MetaTypeImpl<BW::Vector4> base;

	public:
		Vector4MetaType():
			base()
		{
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			const BW::Vector4 & vec = *cast(value);
			stream << vec.x << g_separator << vec.y << g_separator << vec.z << g_separator << vec.w;
			return stream.good();
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if (!stream.good())
			{
				return false;
			}
			BW::Vector4 & vec = *cast(value);
			char separator;
			stream >> vec.x >> separator >> vec.y >> separator >> vec.z >> separator >> vec.w;
			return !stream.fail();
		}

	private:
		static BW::Vector4* cast(void* value)
		{
			return static_cast<BW::Vector4*>(value);
		}

		static const BW::Vector4* cast(const void* value)
		{
			return static_cast<const BW::Vector4*>(value);
		}

	};
}


//==============================================================================
class BWCorePlugin
	: public PluginMain
{
public:
	//==========================================================================
	BWCorePlugin( IComponentContext & contextManager )
		: bwTypeSerializer_( new BWTypeSerializer )
		, bwVector2MetaType_( new Vector2MetaType )
		, bwVector3MetaType_( new Vector3MetaType )
		, bwVector4MetaType_( new Vector4MetaType )
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

	void Initialise( IComponentContext & contextManager )
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
	void Unload( IComponentContext & contextManager ) override
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
	std::unique_ptr< Vector2MetaType > bwVector2MetaType_;
	std::unique_ptr< Vector3MetaType > bwVector3MetaType_;
	std::unique_ptr< Vector4MetaType > bwVector4MetaType_;
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
