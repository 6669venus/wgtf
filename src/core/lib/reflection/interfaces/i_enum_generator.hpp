#ifndef I_ENUM_GENERATOR_HPP
#define I_ENUM_GENERATOR_HPP

class ObjectHandle;
class Collection;

class IEnumGenerator
{
public:
	virtual ~IEnumGenerator() {}
	virtual Collection getCollection( const ObjectHandle & provider ) = 0;
};

#endif //I_ENUM_GENERATOR_HPP