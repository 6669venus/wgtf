#pragma once
#include "core_dependency_system/di_ref.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "i_script_object_definition_registry.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <map>
#include <mutex>


class IClassDefinition;
class IComponentContext;
class IDefinitionHelper;
class IDefinitionManager;
struct ScriptObjectDefinitionDeleter;


/**
 *	Stores an IClassDefinition for each PyScript::ScriptObject instance.
 *	So that duplicate IClassDefinitions are created for the same
 *	PyScript::ScriptObject instance.
 */
class ScriptObjectDefinitionRegistry: public Implements<IScriptObjectDefinitionRegistry>
{
public:
	ScriptObjectDefinitionRegistry( IComponentContext& context );
	virtual ~ScriptObjectDefinitionRegistry();

	void init();
	void fini();

	/**
	 *	Find an existing or add a new definition for the given object.
	 *	Definitions will automatically be removed when the reference count
	 *	on the std::shared_ptr reaches 0.
	 *	@param object the PyScript::ScriptObject for which to find the definition.
	 *		Must not be null.
	 *	@return an existing definition or a newly added definition.
	 */
	virtual std::shared_ptr<IClassDefinition> getDefinition( const PyScript::ScriptObject& object ) override;
	virtual const RefObjectId & getID(
		const PyScript::ScriptObject & object ) override;


private:
	void removeDefinition( const PyScript::ScriptObject& object, const IClassDefinition* definition );

	friend struct ScriptObjectDefinitionDeleter;

	/**
	 *	Key compare functor.
	 *	Need to do a deep compare on PyScript::ScriptObject to prevent getting
	 *	copies of the same object added to the map.
	 */
	class ScriptObjectCompare
	{
	public:
		bool operator()( const PyScript::ScriptObject & a,
			const PyScript::ScriptObject & b ) const
		{
			return a.compareTo( b, PyScript::ScriptErrorPrint() ) < 0;
		}
	};
	typedef std::map< PyScript::ScriptObject,
		std::weak_ptr< IClassDefinition >,
		ScriptObjectCompare > DefinitionMap;
	DefinitionMap definitions_;
	std::mutex definitionsMutex_;
	IComponentContext& context_;
	DIRef< IDefinitionManager > definitionManager_;
	std::unique_ptr< IDefinitionHelper > definitionHelper_;
	typedef std::map< PyScript::ScriptObject,
		RefObjectId,
		ScriptObjectCompare > IDMap;
	IDMap idMap_;
};
