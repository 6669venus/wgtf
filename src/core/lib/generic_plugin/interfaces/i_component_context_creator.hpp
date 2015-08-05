#ifndef I_COMPONENT_CONTEXT_CREATOR_HPP
#define I_COMPONENT_CONTEXT_CREATOR_HPP

class IInterface;

class IComponentContextCreator
{
public:
	virtual ~IComponentContextCreator() {}
	virtual const char * getType() const = 0;
	virtual IInterface * createContext( const wchar_t * contextId ) = 0;
};

#endif ///I_COMPONENT_CONTEXT_CREATOR_HPP