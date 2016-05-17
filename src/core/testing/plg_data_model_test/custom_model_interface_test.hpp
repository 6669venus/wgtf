#ifndef CUSTOM_MODEL_INTERFACE_TEST__HPP
#define CUSTOM_MODEL_INTERFACE_TEST__HPP

#include <memory>

namespace wgt
{
class IComponentContext;
class IView;

class CustomModelInterfaceTest
{
public:
	CustomModelInterfaceTest();
	~CustomModelInterfaceTest();

	void initialise( IComponentContext & contextManager );
	void fini( IComponentContext & contextManager );

private:
	std::unique_ptr< IView > testView_;
};

}
#endif//CUSTOM_MODEL_INTERFACE_TEST__HPP
