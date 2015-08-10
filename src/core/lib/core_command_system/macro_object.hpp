#ifndef MACRO_OBJECT_HPP
#define MACRO_OBJECT_HPP

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"

class ICommandManager;
class IDefinitionManager;
class MacroObject;


class MacroEditObject
{
	DECLARE_REFLECTED
public:
	friend MacroObject;
	MacroEditObject();
	const int & subCommandIndex() const;
	const char *  propertyPath() const;
	const Variant & value() const;

private:
	void subCommandIndex( const int & index);
	void propertyPath( const char *  propertyPath );
	void value( const Variant & value );

	int subCommandIndex_;
	std::string propertyPath_;
	Variant value_;
};


class MacroObject
{
	DECLARE_REFLECTED
public:
	MacroObject();
	void init( ICommandManager& commandSystem, IDefinitionManager & defManager, const char * cmdId );
	const ObjectHandle & getContextObjects() const;
	const ObjectHandle & getContextObject() const;
	void setContextObject( const ObjectHandle & obj );
	ObjectHandle executeMacro() const;
	ObjectHandle createEditData() const;
	ObjectHandle updateMacro() const;

private:
	ICommandManager* commandSystem_;
	IDefinitionManager* pDefManager_;
	std::string cmdId_;
	std::string macroName_;
	//TODO: http://jira.bigworldtech.com/browse/NGT-434
	mutable ObjectHandle contextList_;
	ObjectHandle currentContextObj_;
	mutable ObjectHandle macroEditObjectList_;
};
#endif // MACRO_OBJECT_HPP
