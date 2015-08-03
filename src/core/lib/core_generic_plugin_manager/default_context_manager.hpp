#ifndef DEFAULT_COMPONENT_CONTEXT_HPP
#define DEFAULT_COMPONENT_CONTEXT_HPP

#include "generic_plugin/interfaces/i_component_context.hpp"
#include "variant/type_id.hpp"

#include <set>
#include <map>

class RTTIHelper;

class DefaultComponentContext
	: public IComponentContext
{
public:
	explicit DefaultComponentContext( IComponentContext * parentContext = NULL );
	~DefaultComponentContext();

	IInterface * registerInterfaceImpl(
		const TypeId &, IInterface * pImpl,
		ContextRegState regState ); 

	bool deregisterInterface( IInterface * typeId );

	void * queryInterface( const TypeId & );

	void queryInterface(
		const TypeId &,
		std::vector< void * > & o_Impls );

	void registerListener( IComponentContextListener & listener );
	void deregisterListener( IComponentContextListener & listener );

private:
	typedef std::multimap< const TypeId, RTTIHelper * > InterfaceMap;
	InterfaceMap										interfaces_;
	std::set< IInterface * >								registeredInterfaces_;
	IComponentContext *									parentContext_;
	std::vector< IComponentContextListener * >			listeners_;
};

#endif
