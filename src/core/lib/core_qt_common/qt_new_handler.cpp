#include "qt_new_handler.hpp"

std::set< void * > & getQtInPlaceNewCollection()
{
	static std::set< void * > s_QtInPlaceNewed;
	return s_QtInPlaceNewed;
}