#ifndef I_ENGINE_ADAPTER
#define I_ENGINE_ADAPTER


class IComponentContext; 

class IEngineAdapter
{
public:

	//! \brief call to instantiate main tools application
	virtual bool initApp(const char* project) = 0;

	//! \brief call to register all component types with the framework
	virtual bool registerComponentTypes(IComponentContext & contextManager) = 0;

};

#endif // I_ENGINE_ADAPTER