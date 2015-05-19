#ifndef DEFAULT_CONTEXT_MANAGER_HPP
#define DEFAULT_CONTEXT_MANAGER_HPP

#include "interfaces/i_context_manager.hpp"
#include "variant/type_id.hpp"

#include <set>
#include <map>

class RTTIHelper;

class DefaultContextManager
	: public IContextManager
{
public:
	DefaultContextManager( IContextManager * parentContext = NULL );
	~DefaultContextManager();

	IInterface * registerInterfaceImpl(
		const TypeId &, IInterface * pImpl,
		ContextRegState regState ); 

	bool deregisterInterface( IInterface * typeId );

	void * queryInterface( const TypeId & );

	void queryInterface(
		const TypeId &,
		std::vector< void * > & o_Impls );

	void registerListener( IContextManagerListener & listener );
	void deregisterListener( IContextManagerListener & listener );

private:
	typedef std::multimap< const TypeId, RTTIHelper * > InterfaceMap;
	InterfaceMap										interfaces_;
	std::set< IInterface * >								registeredInterfaces_;
	IContextManager *									parentContext_;
	std::vector< IContextManagerListener * >			listeners_;
};

#endif