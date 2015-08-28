#ifndef QT_FRAMEWORK_ADAPTER_HPP
#define QT_FRAMEWORK_ADAPTER_HPP

#include "qt_framework.hpp"


class QtFrameworkAdapter
	: public QtFrameworkAdapter
{
public:
	QtFrameworkAdapter();
	virtual ~QtFrameworkAdapter();

	std::unique_ptr< IView > createView( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;
	std::unique_ptr< IWindow > createWindow( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;
};

#endif
