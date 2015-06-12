#ifndef IPLUGIN_CONTEXT_CREATOR
#define IPLUGIN_CONTEXT_CREATOR

class IInterface;

class IPluginContextCreator
{
public:
	virtual ~IPluginContextCreator() {}
	virtual const char * getType() const = 0;
	virtual IInterface * createContext( const wchar_t * contextId ) = 0;
};

#endif ///IPLUGIN_CONTEXT_CREATOR