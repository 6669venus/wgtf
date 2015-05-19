#ifndef SHARED_CONTROLS_HPP
#define SHARED_CONTROLS_HPP

class IDefinitionManager;
class CommandSystemProvider;

class SharedControls
{
public:
	static void init();
	static void initDefs(
		IDefinitionManager & definitionManager );
	static void initCommands(
		CommandSystemProvider & commandSystemProvider );
};

#endif //SHARED_CONTROLS_HPP