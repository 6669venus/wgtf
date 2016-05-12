#ifndef CUSTOM_MODEL_INTERFACE_TEST__HPP
#define CUSTOM_MODEL_INTERFACE_TEST__HPP

#include <memory>
#include "core_dependency_system/depends.hpp"

class IComponentContext;
class IView;

namespace wgt
{
	class IViewCreator;
};

class CustomModelInterfaceTest
	: public Depends< wgt::IViewCreator >
{
public:
	CustomModelInterfaceTest(IComponentContext & );
	~CustomModelInterfaceTest();

	void initialise( IComponentContext & contextManager );
	void fini( IComponentContext & contextManager );

private:
	std::unique_ptr< IView > testView_;
};

#endif//CUSTOM_MODEL_INTERFACE_TEST__HPP