#ifndef QT_ACTION_MANAGER_HPP
#define QT_ACTION_MANAGER_HPP

class IAction;

#include <functional>
#include <map>
#include <memory>

struct QtActionData;

class QtActionManager
{
public:
	QtActionManager();
	~QtActionManager();

	void initialise();
	
	std::unique_ptr< IAction > createAction(
		const char * id,
		std::function<void()> func,
		std::function<bool()> enableFunc );

	bool registerActionData( const char * id, 
		std::unique_ptr< QtActionData > & actionData );

private:
	std::map< std::string, std::unique_ptr< QtActionData > > actionData_;
};

#endif//ACTION_MANAGER_HPP