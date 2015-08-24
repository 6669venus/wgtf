#ifndef REFLECTION_METHOD_UTILITIES_HPP
#define REFLECTION_METHOD_UTILITIES_HPP

#include "core_variant/type_id.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_reflection/reflected_method_parameters.hpp"


/*
The utilities consist of ReflectedMethodSpecialisation template classes, MethodReturnSplitter and the
ReflectedMethodFactory. The ReflectedMethodSpecialisation implements the ReflectedMethod and handles different
return types and different combinations and number of parameters. The return value is handled by the
MethodReturnSplitter and returns a empty Variant in the case of a void method. ReflectedMethodFactory instantiated the
different ReflectedMethodSpecialisation classes by deducing types from a method as parameter.
ReflectedMethodSpecialisation and ReflectedMethodFactory entries are created by the RM_METHOD_SPECIALISATION_ and
RM_CREATE_FACTORY_METHOD macros.


Example of a generated ReflectedMethodSpecialisation:

template<class ClassType, class ReturnType, class Parameter0Type, class Parameter1Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType, Parameter0Type, Parameter1Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )( Parameter0Type, Parameter1Type );

	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method )
	{}

	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType& obj = *object.getBase<ClassType>();
		ReflectedMethodParameterWrapper<std::decay<Parameter0Type>::type,
			std::is_reference<Parameter0Type>::value> p0 = parameters[0];
		ReflectedMethodParameterWrapper<std::decay<Parameter1Type>::type,
			std::is_reference<Parameter1Type>::value> p1 = parameters[1];
		methodWithoutParameters = std::bind( method, std::ref( obj ), std::ref( p0() ), std::ref( p1() ) );
		return MethodReturnSplitter::invoke();
	}

	size_t parameterCount() const override { return 2; }

	MethodType method;
};


Example of a generated factory method:

template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type>
static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
	Parameter1Type, Parameter2Type ) )
{
	return new ReflectedMethodSpecialisation<ClassType, ReturnType,
		Parameter1Type, Parameter2Type>( name, method );
}
*/


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
	Variant invoke() { methodWithoutParameters(); return Variant(); }
	std::function<void( void )> methodWithoutParameters;
};


template<class Type, bool Reference = false>
struct ReflectedMethodParameterWrapper
{
	ReflectedMethodParameterWrapper( const Variant& variant )
	{
		ObjectHandle handle;
		value = variant.tryCast<ObjectHandle>( handle ) ? *handle.getBase<Type>() : variant.cast<Type>();
	}

	Type& operator()() { return value; }
	Type value;
};


template<class Type>
struct ReflectedMethodParameterWrapper<Type, true>
{
	ReflectedMethodParameterWrapper( const Variant& variant )
	{
		ObjectHandle handle = variant.cast<ObjectHandle>();
		pointer = handle.getBase<Type>();
	}

	Type& operator()() { return *pointer; }
	Type* pointer;
};


#define RM_PARAMETER_NONE(...)
#define RM_PARAMETER_0(macro) macro(0)
#define RM_PARAMETER_1(macro) RM_PARAMETER_0(macro), macro(1)
#define RM_PARAMETER_2(macro) RM_PARAMETER_1(macro), macro(2)
#define RM_PARAMETER_3(macro) RM_PARAMETER_2(macro), macro(3)
#define RM_PARAMETER_4(macro) RM_PARAMETER_3(macro), macro(4)
#define RM_PARAMETER_5(macro) RM_PARAMETER_4(macro), macro(5)
#define RM_PARAMETER_6(macro) RM_PARAMETER_5(macro), macro(6)
#define RM_PARAMETER_7(macro) RM_PARAMETER_6(macro), macro(7)
#define RM_PARAMETER_8(macro) RM_PARAMETER_7(macro), macro(8)
#define RM_PARAMETER_9(macro) RM_PARAMETER_8(macro), macro(9)

#define RM_PARAMETER_WITH_PREFIX_NONE(prefix, ...) prefix()
#define RM_PARAMETER_WITH_PREFIX_0(prefix, macro) prefix(), macro(0)
#define RM_PARAMETER_WITH_PREFIX_1(prefix, macro) RM_PARAMETER_WITH_PREFIX_0(prefix, macro), macro(1)
#define RM_PARAMETER_WITH_PREFIX_2(prefix, macro) RM_PARAMETER_WITH_PREFIX_1(prefix, macro), macro(2)
#define RM_PARAMETER_WITH_PREFIX_3(prefix, macro) RM_PARAMETER_WITH_PREFIX_2(prefix, macro), macro(3)
#define RM_PARAMETER_WITH_PREFIX_4(prefix, macro) RM_PARAMETER_WITH_PREFIX_3(prefix, macro), macro(4)
#define RM_PARAMETER_WITH_PREFIX_5(prefix, macro) RM_PARAMETER_WITH_PREFIX_4(prefix, macro), macro(5)
#define RM_PARAMETER_WITH_PREFIX_6(prefix, macro) RM_PARAMETER_WITH_PREFIX_5(prefix, macro), macro(6)
#define RM_PARAMETER_WITH_PREFIX_7(prefix, macro) RM_PARAMETER_WITH_PREFIX_6(prefix, macro), macro(7)
#define RM_PARAMETER_WITH_PREFIX_8(prefix, macro) RM_PARAMETER_WITH_PREFIX_7(prefix, macro), macro(8)
#define RM_PARAMETER_WITH_PREFIX_9(prefix, macro) RM_PARAMETER_WITH_PREFIX_8(prefix, macro), macro(9)

#define RM_STATEMENT_NONE(...) 
#define RM_STATEMENT_0(macro) macro(0)
#define RM_STATEMENT_1(macro) RM_STATEMENT_0(macro) macro(1)
#define RM_STATEMENT_2(macro) RM_STATEMENT_1(macro) macro(2)
#define RM_STATEMENT_3(macro) RM_STATEMENT_2(macro) macro(3)
#define RM_STATEMENT_4(macro) RM_STATEMENT_3(macro) macro(4)
#define RM_STATEMENT_5(macro) RM_STATEMENT_4(macro) macro(5)
#define RM_STATEMENT_6(macro) RM_STATEMENT_5(macro) macro(6)
#define RM_STATEMENT_7(macro) RM_STATEMENT_6(macro) macro(7)
#define RM_STATEMENT_8(macro) RM_STATEMENT_7(macro) macro(8)
#define RM_STATEMENT_9(macro) RM_STATEMENT_8(macro) macro(9)

#define RM_EXPAND(...) __VA_ARGS__
#define RM_JOIN(a, b) RM_EXPAND(a##b)

#define RM_CLASS_PARAMETER_PREFIX() class ClassType, class ReturnType
#define RM_CLASS_PARAMETERS(n) RM_JOIN(RM_PARAMETER_WITH_PREFIX_, n)(RM_CLASS_PARAMETER_PREFIX, RM_CLASS_PARAMETER)
#define RM_CLASS_PARAMETERS_PASSED(n) RM_JOIN(RM_PARAMETER_, n)(RM_CLASS_PARAMETER)
#define RM_CLASS_PARAMETER(n) class Parameter##n##Type
#define RM_CLASS_PARAMETER_WITH_DEFAULTS(n) class Parameter##n##Type=void
#define RM_CLASS_PARAMETERS_WITH_DEFAULTS(n)\
	RM_JOIN(RM_PARAMETER_WITH_PREFIX_, n)(RM_CLASS_PARAMETER_PREFIX, RM_CLASS_PARAMETER_WITH_DEFAULTS)

#define RM_PLAIN_PARAMETER_PREFIX() ClassType, ReturnType
#define RM_PLAIN_PARAMETERS(n) RM_JOIN(RM_PARAMETER_WITH_PREFIX_, n)(RM_PLAIN_PARAMETER_PREFIX, RM_PLAIN_PARAMETER)
#define RM_PLAIN_PARAMETERS_PASSED(n) RM_JOIN(RM_PARAMETER_, n)(RM_PLAIN_PARAMETER)
#define RM_PLAIN_PARAMETER(n) Parameter##n##Type

#define RM_PASS_PARAMETER_PREFIX() std::ref(obj)
#define RM_PASS_PARAMETERS(n) RM_JOIN(RM_PARAMETER_WITH_PREFIX_, n)(RM_PASS_PARAMETER_PREFIX, RM_PASS_PARAMETER)
#define RM_PASS_PARAMETER(n) std::ref(p##n())

#define RM_EXTRACT_PARAMETER_LINES(n) RM_JOIN(RM_STATEMENT_, n)(RM_EXTRACT_PARAMETER_LINE)
#define RM_EXTRACT_PARAMETER_LINE(n)\
	ReflectedMethodParameterWrapper<std::decay<Parameter##n##Type>::type,\
	std::is_reference<Parameter##n##Type>::value> p##n = parameters[n];

#define RM_METHOD_SPECIALISATION_DEFAULT(n, count)\
	template<RM_CLASS_PARAMETERS_WITH_DEFAULTS(n)>\
	struct ReflectedMethodSpecialisation\
	RM_METHOD_SPECIALISATION_BODY(n, count)

#define RM_METHOD_SPECIALISATION_SPECIFIC(n, count)\
	template<RM_CLASS_PARAMETERS(n)>\
	struct ReflectedMethodSpecialisation<RM_PLAIN_PARAMETERS(n)>\
	RM_METHOD_SPECIALISATION_BODY(n, count)

#define RM_METHOD_SPECIALISATION_BODY(n, count)\
		: public MethodReturnSplitter<ReturnType>, public ReflectedMethod\
	{\
		typedef ReturnType(ClassType::*MethodType)(RM_PLAIN_PARAMETERS_PASSED(n));\
		\
		ReflectedMethodSpecialisation(const char* name, MethodType method): ReflectedMethod(name), method(method){}\
		\
		Variant invoke(const ObjectHandle& object, const ReflectedMethodParameters& parameters) override\
		{\
			ClassType& obj = *object.getBase<ClassType>();\
			RM_EXTRACT_PARAMETER_LINES(n)\
			methodWithoutParameters = std::bind(method, RM_PASS_PARAMETERS(n));\
			return MethodReturnSplitter::invoke();\
		}\
		\
		size_t parameterCount() const override {return count;}\
		\
		MethodType method;\
	};


#define RM_FACTORY_CREATE_METHOD(n)\
	template<RM_CLASS_PARAMETERS(n)>\
	static ReflectedMethod* create(const char* name, ReturnType(ClassType::*method)(RM_PLAIN_PARAMETERS_PASSED(n)))\
	{return new ReflectedMethodSpecialisation<RM_PLAIN_PARAMETERS(n)>(name, method);}


RM_METHOD_SPECIALISATION_DEFAULT( 9, 10 )
RM_METHOD_SPECIALISATION_SPECIFIC( 8, 9 )
RM_METHOD_SPECIALISATION_SPECIFIC( 7, 8 )
RM_METHOD_SPECIALISATION_SPECIFIC( 6, 7 )
RM_METHOD_SPECIALISATION_SPECIFIC( 5, 6 )
RM_METHOD_SPECIALISATION_SPECIFIC( 4, 5 )
RM_METHOD_SPECIALISATION_SPECIFIC( 3, 4 )
RM_METHOD_SPECIALISATION_SPECIFIC( 2, 3 )
RM_METHOD_SPECIALISATION_SPECIFIC( 1, 2 )
RM_METHOD_SPECIALISATION_SPECIFIC( 0, 1 )
RM_METHOD_SPECIALISATION_SPECIFIC( NONE, 0 )


struct ReflectedMethodFactory
{
	RM_FACTORY_CREATE_METHOD( 9 )
	RM_FACTORY_CREATE_METHOD( 8 )
	RM_FACTORY_CREATE_METHOD( 7 )
	RM_FACTORY_CREATE_METHOD( 6 )
	RM_FACTORY_CREATE_METHOD( 5 )
	RM_FACTORY_CREATE_METHOD( 4 )
	RM_FACTORY_CREATE_METHOD( 3 )
	RM_FACTORY_CREATE_METHOD( 2 )
	RM_FACTORY_CREATE_METHOD( 1 )
	RM_FACTORY_CREATE_METHOD( 0 )
	RM_FACTORY_CREATE_METHOD( NONE )
};

#endif //REFLECTION_METHOD_UTILITIES_HPP
