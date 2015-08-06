//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  signal.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef SIGNAL_H_
#define SIGNAL_H_

#pragma once

// Bare bones signal implementation
// Future improvements could include:
//	Thread safety
//	disconnect
//	auto-disconnect/scoped disconnect
template<class TFunc>
class Signal
{
public:
	void operator()()
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot();
		}
	}

	template<class TArg1>
	void operator()(TArg1 arg1)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1);
		}
	}

	template<class TArg1, class TArg2>
	void operator()(TArg1 arg1, TArg2 arg2)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2);
		}
	}

	template<class TArg1, class TArg2, class TArg3>
	void operator()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2, arg3);
		}
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	void operator()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2, arg3, arg4);
		}
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	void operator()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg5 arg5)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2, arg3, arg4, arg5);
		}
	}

	void connect(std::function<TFunc> slot)
	{
		m_slots.emplace_back(std::move(slot));
	}

	Signal& operator+=( std::function<TFunc> slot )
	{
		m_slots.emplace_back(std::move(slot));
		return *this;
	}

private:
	std::vector<std::function<TFunc>> m_slots;
};
#endif // SIGNAL_H_