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
    using TFunction = std::function<TFunc>;

public:
    template<typename... TArgs>
    void operator()(TArgs&&... args)
	{
        auto endIter = m_slots.end();
        for (auto iter = m_slots.begin(); iter != endIter; ++iter)
		{
            TFunction & function = iter->second;
            if (function)
                function(std::forward<TArgs>(args)...);
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
    size_t nextConnectionID = 1; // reserve zero value as default. If some object did't connect,
                                 // but called disconnect with zero value, it would't breake other callbacks
    std::unordered_map < size_t, TFunction> m_slots;
};
#endif // SIGNAL_H_
