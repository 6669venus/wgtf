#ifndef I_ENV_SYSTEM
#define I_ENV_SYSTEM

#include <memory>

struct ECGUID
{
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;

	bool operator == (const ECGUID& other) const
	{
		return a == other.a && b == other.b && c == other.c && d == other.d;
	}
};

class IEnvComponent
{
public:
	virtual const ECGUID& getGUID() const = 0;
};

class IEnvState
{
public:
	typedef std::unique_ptr<IEnvComponent> IEnvComponentPtr;
	virtual void add( IEnvComponentPtr ec ) = 0;
	virtual void remove( const ECGUID& guid ) = 0;
	virtual IEnvComponent* query( const ECGUID& guid ) const = 0;
};

class IEnvEventListener
{
public:
	virtual void onAddEnv( IEnvState* state ) = 0;
	virtual void onRemoveEnv( IEnvState* state ) = 0;
	virtual void onSelectEnv( IEnvState* state ) = 0;
};

class IEnvManager
{
public:
	virtual void registerListener( IEnvEventListener* listener ) = 0;
	virtual void deregisterListener( IEnvEventListener* listener ) = 0;

	virtual int addEnv() = 0;
	virtual void removeEnv( int id ) = 0;
	virtual void selectEnv( int id ) = 0;
};

#endif // I_ENV_SYSTEM
