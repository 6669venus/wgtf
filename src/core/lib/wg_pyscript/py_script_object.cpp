#include "pch.hpp"

#include "core_common/ngt_windows.hpp"
#include "py_script_object.hpp"

namespace PyScript
{

/* static */ bool ScriptModule::moduleExists( const char * name )
{
	assert( name );
	
	char buf[ MAX_PATH ];

	FILE * fp = NULL;
	PyObject * pLoader = NULL;

	bool result = (_PyImport_FindModule( name, NULL, buf, sizeof( buf ),
		&fp, &pLoader ) != NULL);

	if (fp)
	{
		fclose( fp );
	}

	Py_XDECREF( pLoader );

	Script::clearError();

	return result;
}


bool ScriptObject::isAttributeSetDisabled( const char * key ) const
{
	ScriptType type = ScriptType::getType( *this );

	// obj.__class__.key
	ScriptObject property = type.getAttribute( key, ScriptErrorClear() );
	if (!property.exists())
	{
		// No property on class type
		return false;
	}
	
	// If type( obj.__class__.key ) returns (type "property")
	if (Py_TYPE( property.get() ) != &PyProperty_Type)
	{
		// Property on class is not a descriptor
		return false;
	}

	// hasattr( obj.__class__.key.__set__ )
	return !type.hasAttribute( "__set__" );
}


bool ScriptObject::isAttributeGetDisabled( const char * key ) const
{
	ScriptType type = ScriptType::getType( *this );

	// obj.__class__.key
	ScriptObject property = type.getAttribute( key, ScriptErrorClear() );
	if (!property.exists())
	{
		// No property on class type
		return false;
	}
	
	// If type( obj.__class__.key ) returns (type "property")
	if (Py_TYPE( property.get() ) != &PyProperty_Type)
	{
		// Property on class is not a descriptor
		return false;
	}

	// hasattr( obj.__class__.key.__get__ )
	return !type.hasAttribute( "__get__" );
}


} // namespace PyScript

// py_script_object.cpp
