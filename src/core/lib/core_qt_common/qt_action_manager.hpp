#ifndef QT_ACTION_MANAGER_HPP
#define QT_ACTION_MANAGER_HPP

#include <functional>
#include <map>
#include <memory>

class IAction;
class QIODevice;
struct QtActionData;

class QtActionManager
{
public:
	QtActionManager();
	~QtActionManager();
	
	std::unique_ptr< IAction > createAction(
		const char * id,
		std::function<void()> func,
		std::function<bool()> enableFunc,
		bool checkable, 
		std::function<bool()> checkedFunc );

	void loadActionData( QIODevice & source );
	bool registerActionData( const char * id, 
		std::unique_ptr< QtActionData > & actionData );

private:
	std::map< std::string, std::unique_ptr< QtActionData > > actionData_;
};

#endif//ACTION_MANAGER_HPP