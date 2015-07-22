//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  game_viewport.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef GAME_VIEWPORT_H
#define GAME_VIEWPORT_H

#include "i_viewport.hpp"
#include "dependency_system/i_interface.hpp"

class GameBinaryLoader;

class GameViewport 
	: public Implements<IViewport>
{
public:

	GameViewport(const GameBinaryLoader& loader);

	virtual void SetGraphicsEnvironment(unsigned long int hwnd) const override;

private:
	const GameBinaryLoader& loader_;
};


#endif // GAME_VIEWPORT_H