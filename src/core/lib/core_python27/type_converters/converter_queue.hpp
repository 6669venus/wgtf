#pragma once
#ifndef _PYTHON_TYPE_CONVERTER_QUEUE_HPP
#define _PYTHON_TYPE_CONVERTER_QUEUE_HPP

#include "core_python27/type_converters/default_converter.hpp"
#include "core_python27/type_converters/dict_converter.hpp"
#include "core_python27/type_converters/list_converter.hpp"
#include "core_python27/type_converters/primitive_converter.hpp"
#include "core_python27/type_converters/tuple_converter.hpp"
#include "core_python27/type_converters/none_converter.hpp"

#include <longintrepr.h>

#include <stack>
#include <string>


namespace PythonType
{


/**
 *	Converts the name of a Python type to a C++ type.
 *	@param scriptObject script object from which to get the type.
 *	@return equivalent C++ type name, if supported by type converters.
 */
TypeId scriptTypeToTypeId( const PyScript::ScriptObject & scriptObject );


/**
 *	Queue for converting ScriptObject<->Variant.
 */
class ConverterQueue
{
public:
	ConverterQueue( IComponentContext & context );

	void init();
	void fini();

private:
	IComponentContext & context_;

	BasicTypeConverters basicTypeConverters_;
	DefaultConverter defaultTypeConverter_;
	Converters allConverters_;

	NoneConverter noneTypeConverter_;
	PrimitiveConverter< int > intTypeConverter_;
	PrimitiveConverter< digit > longTypeConverter_;
	PrimitiveConverter< double > floatTypeConverter_;
	PrimitiveConverter< std::string > strTypeConverter_;
	PrimitiveConverter< std::wstring > unicodeTypeConverter_;
	ListConverter listTypeConverter_;
	TupleConverter tupleTypeConverter_;
	DictConverter dictTypeConverter_;

	IInterface * pTypeConvertersInterface_;
};

} // namespace PythonType


#endif // PYTHON_TYPE_CONVERTER_HPP
