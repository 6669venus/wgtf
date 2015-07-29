#ifndef GAME_ENGINE_ADAPTER_H
#define GAME_ENGINE_ADAPTER_H

#include <memory>
#include <interfaces/i_engine_adapter.hpp>
#include <dependency_system/i_interface.hpp>

class GameBinaryLoader;
class GameTypeProvider;
class GameViewport;
class GameFileSystem; 

class WGSEngineAdapter : public Implements < IEngineAdapter >
{
public:

	WGSEngineAdapter();
	virtual ~WGSEngineAdapter(); 

	bool initApp(const char* project) override;
	bool registerComponentTypes(IComponentContext & contextManager) override;
	
	// pass through API for plug-in loading stages
	bool init(IComponentContext & contextManager);
	void fini(IComponentContext & contextManager);
	void postLoad(IComponentContext & contextManager);
	void unload(IComponentContext & contextManager);

private: 

	std::unique_ptr<GameBinaryLoader>	gameLoader_; 
	std::unique_ptr<GameTypeProvider>	typeProvider_;
	std::unique_ptr<GameViewport>		gameViewport_;
	std::unique_ptr<GameFileSystem>		fileSystem_;

	std::vector< IInterface * >			interfaces_;
};

#endif // GAME_ENGINE_ADAPTER_H