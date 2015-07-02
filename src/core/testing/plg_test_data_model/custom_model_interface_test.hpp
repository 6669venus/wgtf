#ifndef CUSTOM_MODEL_INTERFACE_TEST__HPP
#define CUSTOM_MODEL_INTERFACE_TEST__HPP

#include <memory>

class IContextManager;
class IView;

class CustomModelInterfaceTest
{
public:
	CustomModelInterfaceTest();
	~CustomModelInterfaceTest();

	void initialise( IContextManager & contextManager );

private:
	std::unique_ptr< IView > testView_;
};

#endif//CUSTOM_MODEL_INTERFACE_TEST__HPP