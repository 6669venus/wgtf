#ifndef DISPLAY_OBJECT_HPP
#define DISPLAY_OBJECT_HPP

#include "command_system/command_instance.hpp"

class DisplayObject
{
public:
    DisplayObject();
    void init( IDefinitionManager& defManager, const CommandInstancePtr & instance );
    ObjectHandle getDisplayData() const;
private:
	ObjectHandle data_;

};


#endif //DISPLAY_OBJECT_HPP
