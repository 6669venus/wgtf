#ifndef REFLECTION_CONTROLLER_HPP
#define REFLECTION_CONTROLLER_HPP

#include "reflection/interfaces/i_reflection_controller.hpp"

class IContextManager;

class ReflectionController
	: public Implements< IReflectionController >
{
public:
	ReflectionController();
	~ReflectionController();

	virtual void init( IContextManager & contextManager );

	Variant getValue( const PropertyAccessor & pa ) override;
	void setValue( const PropertyAccessor & pa, const Variant & data ) override;

private:
	class Impl;
	std::unique_ptr< Impl > impl_;
};

#endif //COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP