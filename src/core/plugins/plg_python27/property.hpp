#pragma once
#ifndef PYTHON_PROPERTY_HPP
#define PYTHON_PROPERTY_HPP


#include "core_reflection/base_property.hpp"


namespace ReflectedPython
{


/**
 *	Python-specific property that belongs to a Definition.
 */
class Property : public BaseProperty
{
public:
	Property( const char* name );
};


} // namespace ReflectedPython

#endif // PYTHON_PROPERTY_HPP
