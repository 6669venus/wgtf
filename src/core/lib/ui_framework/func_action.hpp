#ifndef FUNC_ACTION_HPP
#define FUNC_ACTION_HPP

#include "i_action.hpp"

#include <functional>

class FuncAction : public IAction
{
public:
	FuncAction( 
		const char * text, 
		std::function<void()> func,
		std::function<bool()> enableFunc = [] { return true; } );
	virtual ~FuncAction();

	const char * text() const override;
	bool enabled() const override;
	void execute() override;

private:
	std::string text_;
	std::function<void()> func_;
	std::function<bool()> enableFunc_;
};

#endif//FUNC_ACTION_HPP