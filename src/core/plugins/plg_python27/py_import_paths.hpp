#pragma once
#ifndef PY_IMPORT_PATHS_HPP
#define PY_IMPORT_PATHS_HPP


#include <vector>


/**
 *	A helper class to generate a Python import paths from path components.
 */
class PyImportPaths
{
public:
	static const char DEFAULT_DELIMITER = ';';

	PyImportPaths( char delimiter = DEFAULT_DELIMITER );

	const std::string pathsAsString() const;
	PyObject * pathAsObject() const;

	bool empty() const 
	{ return paths_.empty(); }

	void append( const PyImportPaths & other );

	void setDelimiter( char delimiter )
	{ delimiter_ = delimiter; }

private:
	typedef std::vector< std::string > OrderedPaths;
	OrderedPaths paths_;
	char delimiter_;

	void addNonResPath( const std::string & path );
};


#endif // PY_IMPORT_PATHS_HPP
