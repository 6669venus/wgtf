#include "pch.hpp"
#include "python_meta_type.hpp"



bool operator==( const PythonMetaType & lhs, const PythonMetaType & rhs )
{
	return (lhs.typeOrClass_ == rhs.typeOrClass_);
}


bool operator!=( const PythonMetaType & lhs, const PythonMetaType & rhs )
{
	return !operator==( lhs, rhs );
}


PythonMetaType::PythonMetaType()
{
}


PythonMetaType::PythonMetaType( const PyScript::ScriptType & type )
	: typeOrClass_( type )
{
	this->generateName();
}


PythonMetaType::PythonMetaType( const PyScript::ScriptClass & type )
	: typeOrClass_( type )
{
	this->generateName();
}


PythonMetaType::PythonMetaType( const PythonMetaType & other )
	: typeOrClass_( PyScript::ScriptObject( other.typeOrClass_.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE ) )
	, nameCache_( other.nameCache_ )
{
}


PythonMetaType::PythonMetaType( PythonMetaType && other )
	: typeOrClass_( std::move( other.typeOrClass_ ) )
	, nameCache_( std::move( other.nameCache_ ) )
{
}


const char * PythonMetaType::name() const /* override */
{
	return nameCache_.c_str();
}


const PyScript::ScriptObject & PythonMetaType::type() const
{
	return typeOrClass_;
}


PythonMetaType & PythonMetaType::operator=( const PythonMetaType & other )
{
	typeOrClass_ = PyScript::ScriptObject( other.typeOrClass_.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	nameCache_ = other.nameCache_;
	return (*this);
}


PythonMetaType & PythonMetaType::operator=( PythonMetaType && other )
{
	typeOrClass_ = std::move( other.typeOrClass_ );
	nameCache_ = std::move( other.nameCache_ );
	return (*this);
}


void PythonMetaType::generateName()
{
	if (!typeOrClass_.exists())
	{
		nameCache_.clear();
	}
	else if (PyScript::ScriptType::check( typeOrClass_ ))
	{
		// Type type
		// type.__module__ + type.__name__
		PyScript::ScriptType scriptType(
			reinterpret_cast< PyTypeObject * >( typeOrClass_.get() ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		std::string module;
		scriptType.getAttribute( "__module__",
			module,
			PyScript::ScriptErrorPrint() );
		nameCache_ = module;
		nameCache_ += '.';
		nameCache_ += scriptType.name();
	}
	else
	{
		// Class or None type
		// str( type )
		// class_str will form the name from __module__ + __name__
		nameCache_ = typeOrClass_.str( PyScript::ScriptErrorPrint() ).c_str();
	}
}
