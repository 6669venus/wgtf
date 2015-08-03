#ifndef COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP
#define COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP

#include "core_reflection/interfaces/i_reflection_property_setter.hpp"

class ICommandManager;

class CommandSystemReflectionPropertySetter
	: public Implements< IReflectionPropertySetter >
{
public:
	CommandSystemReflectionPropertySetter();
	
	void init( ICommandManager & commandSystemProvider );
	void setDataValue( const PropertyAccessor & pa, const Variant & data ) override;

private:
	ICommandManager * commandSystemProvider_;
};

#endif //COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP