#ifndef SET_REFLECTED_PROPERTY_ROOT_OBJECT_COMMAND_HPP
#define SET_REFLECTED_PROPERTY_ROOT_OBJECT_COMMAND_HPP

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"


class ReflectedPropertyRootObjectSetter;
class SetReflectedPropertyRootObjectCommand;

class ReflectedPropertyRootObjectCommandArgument
{
	DECLARE_REFLECTED

public:
	ReflectedPropertyRootObjectCommandArgument();

	const ObjectHandle & getValue() const;

	
	void init( ReflectedPropertyRootObjectSetter * pSetter, const ObjectHandle & value );

	static const char * sourceName();
	static const char * valueName();

private:
	friend SetReflectedPropertyRootObjectCommand;
	void setValue(const ObjectHandle & value);

	ReflectedPropertyRootObjectSetter* pSetter_;
	ObjectHandle		value_;
	static const char * s_SourceName;
	static const char * s_ValueName;
};



class SetReflectedPropertyRootObjectCommand
	: public Command
{

public:
	const char * getId() const override;
	ObjectHandle execute(
		const ObjectHandle & arguments ) const override;

	
};

#endif //SET_REFLECTED_PROPERTY_ROOT_OBJECT_COMMAND_HPP