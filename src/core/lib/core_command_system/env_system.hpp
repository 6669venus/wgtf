#ifndef ENV_SYSTEM
#define ENV_SYSTEM

#include "i_env_system.hpp"
#include <vector>

class EnvState : public IEnvState
{
public:
	EnvState() {}
	virtual ~EnvState() {}

	virtual void add( IEnvComponentPtr ec ) override;
	virtual void remove( const ECGUID& guid ) override;
	virtual IEnvComponent* query( const ECGUID& guid ) const override;

private:
	typedef std::vector<IEnvComponentPtr> Components;
	Components components_;
};

class EnvManager : public IEnvManager
{
public:
	EnvManager() : idx_(0) {}
	virtual ~EnvManager() {}

	virtual void registerListener( IEnvEventListener* listener ) override;
	virtual void deregisterListener( IEnvEventListener* listener ) override;

	virtual int addEnv() override;
	virtual void removeEnv( int id ) override;
	virtual void selectEnv( int id ) override;

private:
	typedef std::unique_ptr<IEnvState> IEnvStatePtr;
	typedef std::vector<std::pair<int, IEnvStatePtr>> StateVec;
	StateVec stateVec_;
	int idx_;

	typedef std::vector<IEnvEventListener*> Listeners;
	Listeners listeners_;
};

#endif // ENV_SYSTEM