#include "qt_framework_adapter.hpp"

QtFrameWorkAdapter::QtFrameWorkAdapter()
{

}

QtFrameWorkAdapter::~QtFrameWorkAdapter()
{
}

std::unique_ptr< IView > QtFrameWorkAdapter::createView( 
	const char * resource, ResourceType type,
	const ObjectHandle & context )
{
	// TODO: This function assumes the resource is a qml file

	QUrl qUrl;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
		qUrl = QUrl::fromLocalFile( resource );
		break;

	case IUIFramework::ResourceType::Url:
		qUrl = QUrl( resource );
		break;

	default:
		return nullptr;
	}

	auto scriptObject = scriptingEngine_->createScriptObject( context );

	auto view = new QmlView( *qmlEngine_ );

	if (scriptObject)
	{
		view->setContextObject( scriptObject );
	}
	else
	{
		auto source = toQVariant( context );
		view->setContextProperty( QString( "source" ), source );
	}

	view->load( qUrl );
	return std::unique_ptr< IView >( view );
}

std::unique_ptr< IWindow > QtFrameWorkAdapter::createWindow( 
	const char * resource, ResourceType type,
	const ObjectHandle & context )
{
	// TODO: This function assumes the resource is a ui file containing a QMainWindow

	std::unique_ptr< QIODevice > device;
	IWindow* window = nullptr;
	switch (type)
	{
	case IUIFramework::ResourceType::File:
		{
			device.reset( new QFile( resource ) );
			device->open( QFile::ReadOnly );
			assert( device != nullptr );
			window = new QtWindow( *this, *device );
			device->close();
		}
		break;
	case IUIFramework::ResourceType::Url:
		{
			QUrl qUrl( resource );
			auto scriptObject = scriptingEngine_->createScriptObject( context );
			auto qmlWindow = new QmlWindow( *this, *qmlEngine_ );

			if (scriptObject)
			{
				qmlWindow->setContextObject( scriptObject );
			}
			else
			{
				auto source = toQVariant( context );
				qmlWindow->setContextProperty( QString( "source" ), source );
			}

			qmlWindow->load( qUrl );
			window = qmlWindow;
		}
		break;

	default:
		return nullptr;
	}

	return std::unique_ptr< IWindow >( window );
}

