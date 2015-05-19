#include "func_action.hpp"

FuncAction::FuncAction( 
	const char * text, 
	std::function<void()> func,
	std::function<bool()> enableFunc )
	: text_( text )
	, func_( func )
	, enableFunc_( enableFunc )
{

}

FuncAction::~FuncAction()
{

}

const char * FuncAction::text() const
{
	return text_.c_str();
}

bool FuncAction::enabled() const
{
	return enableFunc_();
}

void FuncAction::execute()
{
	func_();
}