#ifndef COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP
#define COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP

#include "reflection/interfaces/i_reflection_property_setter.hpp"

class CommandSystemProvider;

class CommandSystemReflectionPropertySetter
	: public Implements< IReflectionPropertySetter >
{
public:
	CommandSystemReflectionPropertySetter();
	
	void init( CommandSystemProvider & commandSystemProvider );
	void setDataValue( const PropertyAccessor & pa, const Variant & data ) override;

private:
	CommandSystemProvider * commandSystemProvider_;
};

#endif //COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP