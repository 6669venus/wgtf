#ifndef I_CONTEXT_MANAGER_HPP
#define I_CONTEXT_MANAGER_HPP

#include <vector>
#include <functional>
#include "dependency_system/i_interface.hpp"
#include "variant/type_id.hpp"

class IInterface;
class IPluginContextCreator;

class IContextManagerListener
{
public:
	typedef std::function< void * ( const TypeId & ) > InterfaceCaster;

	virtual ~IContextManagerListener() {}
	virtual void onPluginContextRegistered( IPluginContextCreator * ) {}
	virtual void onPluginContextDeregistered( IPluginContextCreator * ) {}

	virtual void onInterfaceRegistered( InterfaceCaster & ) {}
	virtual void onInterfaceDeregistered( InterfaceCaster & ) {}
};

class IContextManager
{
public:
	virtual ~IContextManager() {}

	enum ContextRegState
	{
		Reg_Local,
		Reg_Parent,
		Reg_Global = Reg_Parent
	};

	template< class T >
	IInterface * registerInterface( T * pImpl,
		bool transferOwnership = true, ContextRegState regState = Reg_Global )
	{
		return registerInterfaceImpl(
			TypeId::getType< T >(), new InterfaceHolder< T >( pImpl, transferOwnership ), regState );
	}

	virtual IInterface * registerInterfaceImpl(
		const TypeId &, IInterface * pImpl,
		ContextRegState regState ) = 0;


	virtual bool deregisterInterface( IInterface * typeId ) = 0;

	virtual void * queryInterface( const TypeId & ) = 0;

	template< class T >
	T * queryInterface()
	{
		return reinterpret_cast< T * >(
			queryInterface( typeid( T ).name() ) );
	}

	virtual void queryInterface(
		const TypeId &, std::vector< void * > & o_Impls ) = 0;

	template< class T >
	void queryInterface( std::vector< T * > & o_Impls )
	{
		queryInterface(
			typeid( T ).name(),
			reinterpret_cast< std::vector< void * > & >( o_Impls ) );
	}

	virtual void registerListener( IContextManagerListener & listener ) = 0;
	virtual void deregisterListener( IContextManagerListener & listener ) = 0;
};

#endif //I_CONTEXT_MANAGER_HPP
