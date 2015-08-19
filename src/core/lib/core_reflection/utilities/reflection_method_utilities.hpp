#ifndef REFLECTION_METHOD_UTILITIES_HPP
#define REFLECTION_METHOD_UTILITIES_HPP

#include "core_variant/type_id.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_reflection/reflected_method_parameters.hpp"


template<class ReturnType>
struct MethodReturnSplitter
{
protected:
	Variant invoke() { return methodWithoutParameters(); }
	std::function<ReturnType( void )> methodWithoutParameters;
};


template<>
struct MethodReturnSplitter<void>
{
	Variant invoke() { methodWithoutParameters(); return 0; }
	std::function<void( void )> methodWithoutParameters;
};


template<class ClassType, class ReturnType,
	class Parameter1Type = void, class Parameter2Type = void, class Parameter3Type = void,
	class Parameter4Type = void, class Parameter5Type = void, class Parameter6Type = void,
	class Parameter7Type = void, class Parameter8Type = void, class Parameter9Type = void>
struct ReflectedMethodSpecialisation
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type,
		Parameter7Type, Parameter8Type, Parameter9Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		Parameter2Type& p2 = *parameters[1].getBase<Parameter2Type>();
		Parameter3Type& p3 = *parameters[2].getBase<Parameter3Type>();
		Parameter4Type& p4 = *parameters[3].getBase<Parameter4Type>();
		Parameter5Type& p5 = *parameters[4].getBase<Parameter5Type>();
		Parameter6Type& p6 = *parameters[5].getBase<Parameter6Type>();
		Parameter7Type& p7 = *parameters[6].getBase<Parameter7Type>();
		Parameter8Type& p8 = *parameters[7].getBase<Parameter8Type>();
		Parameter9Type& p9 = *parameters[8].getBase<Parameter9Type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ), std::ref( p3 ), std::ref( p4 ), std::ref( p5 ), std::ref( p6 ), std::ref( p7 ), std::ref( p8 ), std::ref( p9 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 9;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type, class Parameter3Type,
	class Parameter4Type, class Parameter5Type, class Parameter6Type,
	class Parameter7Type, class Parameter8Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type, Parameter2Type, Parameter3Type,
	Parameter4Type, Parameter5Type, Parameter6Type,
	Parameter7Type, Parameter8Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type,
		Parameter7Type, Parameter8Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		Parameter2Type& p2 = *parameters[1].getBase<Parameter2Type>();
		Parameter3Type& p3 = *parameters[2].getBase<Parameter3Type>();
		Parameter4Type& p4 = *parameters[3].getBase<Parameter4Type>();
		Parameter5Type& p5 = *parameters[4].getBase<Parameter5Type>();
		Parameter6Type& p6 = *parameters[5].getBase<Parameter6Type>();
		Parameter7Type& p7 = *parameters[6].getBase<Parameter7Type>();
		Parameter8Type& p8 = *parameters[7].getBase<Parameter8Type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ), std::ref( p3 ), std::ref( p4 ), std::ref( p5 ), std::ref( p6 ), std::ref( p7 ), std::ref( p8 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 8;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type, class Parameter3Type,
	class Parameter4Type, class Parameter5Type, class Parameter6Type,
	class Parameter7Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type, Parameter2Type, Parameter3Type,
	Parameter4Type, Parameter5Type, Parameter6Type,
	Parameter7Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type,
		Parameter7Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		Parameter2Type& p2 = *parameters[1].getBase<Parameter2Type>();
		Parameter3Type& p3 = *parameters[2].getBase<Parameter3Type>();
		Parameter4Type& p4 = *parameters[3].getBase<Parameter4Type>();
		Parameter5Type& p5 = *parameters[4].getBase<Parameter5Type>();
		Parameter6Type& p6 = *parameters[5].getBase<Parameter6Type>();
		Parameter7Type& p7 = *parameters[6].getBase<Parameter7Type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ), std::ref( p3 ), std::ref( p4 ), std::ref( p5 ), std::ref( p6 ), std::ref( p7 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 7;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type, class Parameter3Type,
	class Parameter4Type, class Parameter5Type, class Parameter6Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type, Parameter2Type, Parameter3Type,
	Parameter4Type, Parameter5Type, Parameter6Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		Parameter2Type& p2 = *parameters[1].getBase<Parameter2Type>();
		Parameter3Type& p3 = *parameters[2].getBase<Parameter3Type>();
		Parameter4Type& p4 = *parameters[3].getBase<Parameter4Type>();
		Parameter5Type& p5 = *parameters[4].getBase<Parameter5Type>();
		Parameter6Type& p6 = *parameters[5].getBase<Parameter6Type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ), std::ref( p3 ), std::ref( p4 ), std::ref( p5 ), std::ref( p6 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 6;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type, class Parameter3Type,
	class Parameter4Type, class Parameter5Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type, Parameter2Type, Parameter3Type,
	Parameter4Type, Parameter5Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		Parameter2Type& p2 = *parameters[1].getBase<Parameter2Type>();
		Parameter3Type& p3 = *parameters[2].getBase<Parameter3Type>();
		Parameter4Type& p4 = *parameters[3].getBase<Parameter4Type>();
		Parameter5Type& p5 = *parameters[4].getBase<Parameter5Type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ), std::ref( p3 ), std::ref( p4 ), std::ref( p5 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 5;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type, class Parameter3Type,
	class Parameter4Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type, Parameter2Type, Parameter3Type,
	Parameter4Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		Parameter2Type& p2 = *parameters[1].getBase<Parameter2Type>();
		Parameter3Type& p3 = *parameters[2].getBase<Parameter3Type>();
		Parameter4Type& p4 = *parameters[3].getBase<Parameter4Type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ), std::ref( p3 ), std::ref( p4 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 4;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type, class Parameter3Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type, Parameter2Type, Parameter3Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type, Parameter3Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		Parameter2Type& p2 = *parameters[1].getBase<Parameter2Type>();
		Parameter3Type& p3 = *parameters[2].getBase<Parameter3Type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ), std::ref( p3 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 3;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type, Parameter2Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type, Parameter2Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		//Parameter1Type& p1 = *parameters[0].getBase<Parameter1Type>();
		//std::decay<Parameter1Type>::type& p1 = *parameters[0].getBase<Parameter1Type>();
		//auto p1 = ParameterExtractor<Parameter1Type>::extract( parameters[0] );
		Parameter1Type& p1 = *parameters[0].getBase<std::decay<Parameter1Type>::type>();
		Parameter2Type& p2 = *parameters[1].getBase<std::decay<Parameter2Type>::type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ), std::ref( p2 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 2;
	}


	MethodType method;
};


template<class ClassType, class ReturnType,
	class Parameter1Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType,
	Parameter1Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )(
		Parameter1Type );


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		Parameter1Type& p1 = *parameters[0].getBase<std::decay<Parameter1Type>::type>();
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p1 ) );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 1;
	}


	MethodType method;
};


template<class ClassType, class ReturnType>
struct ReflectedMethodSpecialisation<ClassType, ReturnType>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )();


	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method ) {}


	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		methodWithoutParameters = std::bind( method, obj );
		return MethodReturnSplitter::invoke();
	}


	int argCount() const
	{
		return 0;
	}


	MethodType method;
};


struct ReflectedMethodFactory
{
	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type, class Parameter3Type,
		class Parameter4Type, class Parameter5Type, class Parameter6Type,
		class Parameter7Type, class Parameter8Type, class Parameter9Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type,
		Parameter7Type, Parameter8Type, Parameter9Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type, Parameter3Type,
			Parameter4Type, Parameter5Type, Parameter6Type,
			Parameter7Type, Parameter8Type, Parameter9Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type, class Parameter3Type,
		class Parameter4Type, class Parameter5Type, class Parameter6Type,
		class Parameter7Type, class Parameter8Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type,
		Parameter7Type, Parameter8Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type, Parameter3Type,
			Parameter4Type, Parameter5Type, Parameter6Type,
			Parameter7Type, Parameter8Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type, class Parameter3Type,
		class Parameter4Type, class Parameter5Type, class Parameter6Type,
		class Parameter7Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type,
		Parameter7Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type, Parameter3Type,
			Parameter4Type, Parameter5Type, Parameter6Type,
			Parameter7Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type, class Parameter3Type,
		class Parameter4Type, class Parameter5Type, class Parameter6Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type, Parameter6Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type, Parameter3Type,
			Parameter4Type, Parameter5Type, Parameter6Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type, class Parameter3Type,
		class Parameter4Type, class Parameter5Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type, Parameter5Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type, Parameter3Type,
			Parameter4Type, Parameter5Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type, class Parameter3Type,
		class Parameter4Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type, Parameter3Type,
		Parameter4Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type, Parameter3Type,
			Parameter4Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type, class Parameter3Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type, Parameter3Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type, Parameter3Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type, class Parameter2Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type, Parameter2Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type, Parameter2Type>( name, method );
	}


	template<class ClassType, class ReturnType,
		class Parameter1Type>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
		Parameter1Type ) )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType,
			Parameter1Type>( name, method );
	}


	template<class ClassType, class ReturnType>
	static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )() )
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType>( name, method );
	}
};

#endif //REFLECTION_METHOD_UTILITIES_HPP
