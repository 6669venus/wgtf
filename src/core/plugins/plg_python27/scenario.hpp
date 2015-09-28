#ifndef SCENARIO_HPP
#define SCENARIO_HPP

#include "core_reflection/reflected_object.hpp"

#include <string>

/**
 *	Test object.
 *	TODO move out of plg_python27
 */
class Scenario
{
	DECLARE_REFLECTED
public:
	std::string name_;
};

#endif // SCENARIO_HPP

