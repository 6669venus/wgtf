#ifndef V0_AUTO_POPULATE_V0_HPP
#define V0_AUTO_POPULATE_V0_HPP

#include "core_dependency_system/i_interface.hpp"

DECLARE_INTERFACE_BEGIN( AutoPopulate, 0, 0 )
	typedef INTERFACE_VERSION( TestInterface, 0, 1 ) InterfaceA;
	typedef INTERFACE_VERSION( TestInterface, 0, 0 ) InterfaceB;

	virtual InterfaceA * getInterfaceA() = 0;
	virtual std::vector< InterfaceB * > getInterfaceBs() = 0;
DECLARE_INTERFACE_END()

#endif //V0_AUTO_POPULATE_V0_HPP
