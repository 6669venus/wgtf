#ifndef RUNNABLE_HPP
#define RUNNABLE_HPP

class Runnable
{
public:
	virtual ~Runnable() {}
	virtual void execute() = 0;
	virtual bool isComplete() const = 0;
};

#endif //RUNNABLE_HPP