#pragma once
#ifndef PYTHON_PROPERTY_HPP
#define PYTHON_PROPERTY_HPP


#include "core_reflection/base_property.hpp"


/**
 *	Python-specific property that belongs to a Definition.
 */
class Property : public BaseProperty
{
public:
	Property( const char* name );
};


#endif // PYTHON_PROPERTY_HPP
