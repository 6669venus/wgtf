#include "core_generic_plugin/generic_plugin.hpp"

#include "qt_copy_paste_manager.hpp"
#include "qt_framework.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"

#include <vector>

#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <QFile>
#include <QTextStream>
#include <QRegExp>

class QtPluginContextCreator
	: public Implements< IComponentContextCreator >
{
public:
	QtPluginContextCreator(QtFramework * qtFramework)
		:qtFramework_( qtFramework )
	{

	}

	IInterface * createContext(const wchar_t * contextId)
	{
		QFile resourcePathFile( getResourcePathFile( contextId ) );
		if (resourcePathFile.open( QFile::ReadOnly | QFile::Text ))
		{
			QTextStream in( &resourcePathFile );
			while (!in.atEnd())
			{
				qtFramework_->addImportPath( in.readLine() );
			}
			resourcePathFile.close();
		}

		return new InterfaceHolder< QtFramework >( qtFramework_, false );
	}

	const char * getType() const
	{
		return typeid(QtFramework).name();
	}

private:
	QString getResourcePathFile( const wchar_t * contextId )
	{
		QString path = QString::fromWCharArray( contextId );
		path.remove( 0, path.lastIndexOf( '/' ) );
		path.prepend(":");
		path.append( "/resource_paths.txt" );
		return path;
	}
	QtFramework * qtFramework_;
};


class QtPlugin
	: public PluginMain
{
public:
	QtPlugin( IComponentContext & contextManager ){}

	bool PostLoad( IComponentContext & contextManager ) override
	{
        qtCopyPasteManager_ = new QtCopyPasteManager();
		types_.push_back(
			contextManager.registerInterface( qtCopyPasteManager_ ) );

		qtFramework_ = new QtFramework();
		contextManager.registerInterface( new QtPluginContextCreator( qtFramework_ ) );

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		auto serializationManager = contextManager.queryInterface<ISerializationManager>();
		auto commandsystem = contextManager.queryInterface<ICommandManager>();
		qtCopyPasteManager_->init( serializationManager, commandsystem );
		qtFramework_->initialise( contextManager );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
        qtCopyPasteManager_->fini();
		qtFramework_->finalise();
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}

		qtFramework_ = nullptr;
        qtCopyPasteManager_ = nullptr;
	}

private:
	QtFramework * qtFramework_;
    QtCopyPasteManager * qtCopyPasteManager_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPlugin )

