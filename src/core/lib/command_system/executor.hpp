#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <memory>

class Runnable;

class Executor
{
public:
	virtual void executeRunnable( std::unique_ptr< Runnable > & runnable ) = 0;
};

#endif //EXECUTOR_HPP