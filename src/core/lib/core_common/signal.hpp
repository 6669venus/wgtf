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

#include <functional>
#include <unordered_map>

// Bare bones signal implementation
// Future improvements could include:
//	Thread safety
//	disconnect
//	auto-disconnect/scoped disconnect
template<class TFunc>
class Signal
{
    typedef std::function<TFunc> TFunction;

public:
    Signal()
        : nextConnectionID(1)
    {
        // reserve zero value as default. If some object did't connect,
        // but called disconnect with zero value, it would't breake other callbacks
    }

    void operator()()
    {
        for (auto& slotNode : m_slots)
        {
            TFunction & slot = slotNode.second;
            if ( slot )
                slot();
        }
    }

    template<class TArg1>
    void operator()(TArg1&& arg1)
    {
        for (auto& slotNode : m_slots)
        {
            TFunction & slot = slotNode.second;
            if ( slot )
                slot(std::forward<TArg1>(arg1));
        }
    }

    template<class TArg1, class TArg2>
    void operator()(TArg1&& arg1, TArg2&& arg2)
    {
        for (auto& slotNode : m_slots)
        {
            TFunction & slot = slotNode.second;
            if ( slot )
                slot(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2));
        }
    }

    template<class TArg1, class TArg2, class TArg3>
    void operator()(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3)
    {
        for (auto& slotNode : m_slots)
        {
            TFunction & slot = slotNode.second;
            if ( slot )
                slot(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg2>(arg3));
        }
    }

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	void operator()(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4)
	{
		for ( auto& slotNode : m_slots )
		{
            TFunction & slot = slotNode.second;
			if ( slot )
				slot(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2),
				std::forward<TArg3>(arg3), std::forward<TArg4>(arg4));
		}
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	void operator()(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5)
	{
		for ( auto& slotNode : m_slots )
		{
            TFunction & slot = slotNode.second;
			if ( slot )
				slot(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
				std::forward<TArg4>(arg4), std::forward<TArg5>(arg5));
		}
	}

	size_t connect(std::function<TFunc> slot)
	{
        m_slots.emplace(nextConnectionID, slot);
        return nextConnectionID++;
	}

    void disconnect(size_t connectionId)
    {
        m_slots.erase(connectionId);
    }

	Signal& operator+=( std::function<TFunc> slot )
	{
        connect(slot);
		return *this;
	}

	void clear()
	{
		m_slots.clear();
	}

private:
    size_t nextConnectionID;
    std::unordered_map < size_t, TFunction> m_slots;
};
#endif // SIGNAL_H_
