#pragma once
#ifndef _PYTHON_TYPE_CONVERTER_QUEUE_HPP
#define _PYTHON_TYPE_CONVERTER_QUEUE_HPP

#include "core_python27/type_converters/dict_converter.hpp"
#include "core_python27/type_converters/list_converter.hpp"
#include "core_python27/type_converters/primitive_converter.hpp"
#include "core_python27/type_converters/tuple_converter.hpp"
#include "core_python27/type_converters/type_converter.hpp"

#include <longintrepr.h>

#include <stack>
#include <string>


namespace PythonType
{

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

	PythonTypeConverters typeConverters_;

	PrimitiveConverter< bool > boolTypeConverter_;
	PrimitiveConverter< int > intTypeConverter_;
	PrimitiveConverter< digit > longTypeConverter_;
	PrimitiveConverter< double > doubleTypeConverter_;
	PrimitiveConverter< std::string > stringTypeConverter_;
	PrimitiveConverter< std::wstring > unicodeTypeConverter_;
	ListConverter listTypeConverter_;
	TupleConverter tupleTypeConverter_;
	DictConverter dictTypeConverter_;
	TypeConverter defaultTypeConverter_;

	IInterface * pTypeConvertersInterface_;
};

} // namespace PythonType


#endif // PYTHON_TYPE_CONVERTER_HPP
