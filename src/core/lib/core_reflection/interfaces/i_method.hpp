#ifndef I_METHOD_HPP
#define I_METHOD_HPP

class ObjectHandle;
class ReflectedMethodParameters;
class Variant;

class IMethod
{
public:
	virtual ~IMethod() {}
	virtual Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) = 0;
};

#endif // I_METHOD_HPP
