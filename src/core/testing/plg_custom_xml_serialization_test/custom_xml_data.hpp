#ifndef CUSTOM_XML_DATA_HPP
#define CUSTOM_XML_DATA_HPP
#include <string>
#include "wg_types/vector3.hpp"

namespace wgt
{
struct CustomXmlData
{
	std::string name_;
	std::string filename_;
	std::string createdBy_;
	bool visibility_;
	Vector3 position_;

	bool operator==( const CustomXmlData& tdo ) const;
};
} // end namespace wgt
#endif //CUSTOM_XML_DATA_HPP
