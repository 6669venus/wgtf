#ifndef MACRO_OBJECT_HPP
#define MACRO_OBJECT_HPP

#include "reflection/object_handle.hpp"
#include "reflection/reflected_object.hpp"

class ICommandManager;
class IDefinitionManager;
class CompoundCommandArgument;
class CompoundCommand;
class MacroObject;


class MacroEditObject
{
	DECLARE_REFLECTED
public:
	friend MacroObject;
	MacroEditObject();
	const int & commandInstanceIndex() const;
	const char *  objectId() const;
	const char *  propertyPath() const;
	const char *  propertyType() const;
	const MetaType * valueType() const;
	const Variant & value() const;

private:
	void commandInstanceIndex( const int & index);
	void objectId( const char *  id );
	void propertyPath( const char *  propertyPath );
	void propertyType( const char *  propertyType );
	void valueType( const MetaType * valueType );
	void value( const Variant & value );

	int commandInstanceIndex_;
	std::string objectId_;
	std::string propertyPath_;
	std::string propertyTypeName_;
	const MetaType*	valueType_;
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
