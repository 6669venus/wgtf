#ifndef SHARED_CONTROLS_HPP
#define SHARED_CONTROLS_HPP

class IDefinitionManager;
class ICommandManager;

class SharedControls
{
public:
	static void init();
	static void initDefs(
		IDefinitionManager & definitionManager );
	static void initCommands(
		ICommandManager & commandSystemProvider );
};

#endif //SHARED_CONTROLS_HPP