#pragma once
#ifndef _PYTHON_LOADER_HPP
#define _PYTHON_LOADER_HPP


class IComponentContext;
class ObjectHandle;


namespace PythonLoader
{

/**
 *	Import a module from Python and load data from XML.
 *	@param context
 *	@param path path to scripts.
 *	@param moduleName name of module to be imported.
 *	@param outModule output module imported by Python scripts.
 *	@return true on success.
 */
bool createPythonObjects( IComponentContext & context,
	const wchar_t * path,
	const char * moduleName,
	ObjectHandle & outModule );

} // namespace PythonLoader

#endif // _PYTHON_LOADER_HPP
