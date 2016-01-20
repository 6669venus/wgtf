#pragma once
#ifndef _PYTHON_LOADER_HPP
#define _PYTHON_LOADER_HPP


class Collection;
class IComponentContext;
class ObjectHandle;


namespace PythonLoader
{

/**
 *	Import a module from Python and load data from XML.
 *	@param context
 *	@param outRootPythonObject root of Python scripts.
 *	@param outMapsSettingsXMLDataCollection root of XML data.
 *	@return true on success.
 */
bool createPythonObjects( IComponentContext & context,
	ObjectHandle & outRootPythonObject,
	Collection & outMapsSettingsXMLDataCollection );

} // namespace PythonLoader

#endif // _PYTHON_LOADER_HPP
