#include "pch.hpp"

#include "py_import_paths.hpp"
#include "py_script_object.hpp"
#include "core_string_utils/string_utils.hpp"

#include <sstream>


/**
 *	Constructor.
 *
 *	@param delimiter 	The delimiter to use when converting to a paths string.
 */
PyImportPaths::PyImportPaths( char delimiter ) :
		paths_(),
		delimiter_( delimiter )
{}

PyObject * PyImportPaths::pathAsObject() const
{
	PyObject *pResult;//, *pPath;
	size_t len = paths_.size();
	pResult = PyList_New( len );

	size_t i = 0;
	for (OrderedPaths::const_iterator iPath = paths_.begin();
			iPath != paths_.end(); 
			++iPath)
	{
		std::wstring output;
		if ((*iPath).empty())
		{
			continue;
		}

		std::wstring_convert< Utf16to8Facet > conversion( 
			Utf16to8Facet::create() );

		output = conversion.from_bytes( (*iPath).c_str() );

		// TODO
		//pPath = Script::getData( output.c_str() );
		//if (pPath != NULL) 
		//{
		//	PyList_SetItem(pResult, i, pPath);
		//	++i;
		//}
	}
	return pResult;
}


/**
 *	Return a string containing all the import paths in the format expected by
 *	Python.
 */
const std::string PyImportPaths::pathsAsString() const
{
	std::ostringstream oss;

	bool first = true;
	OrderedPaths::const_iterator iPath = paths_.begin();
	while (iPath != paths_.end())
	{
		if (!first)
		{
			oss << delimiter_;
		}

		oss << *iPath;

		first = false;
		++iPath;
	}

	return oss.str();
}


/**
 *	Adds a raw path.
 */
void PyImportPaths::addNonResPath( const std::string & path )
{
	if (std::find( paths_.begin(), paths_.end(), path) == paths_.end())
	{
		paths_.push_back( path );
	}
}


/**
 *	Appends the input PyImportPaths to this PyImportPaths, ensuring that the other
 *	paths are after the last path in this collection.
 */
void PyImportPaths::append( const PyImportPaths & other )
{
	OrderedPaths::const_iterator iOtherPath = other.paths_.begin();

	while (iOtherPath != other.paths_.end())
	{
		this->addNonResPath( *iOtherPath );
		++iOtherPath;
	}
}

