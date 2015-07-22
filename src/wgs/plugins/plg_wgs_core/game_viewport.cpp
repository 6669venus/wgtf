//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  game_viewport.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "game_common.h"
#include "game_viewport.h"
#include "game_loader.h"

#include <dsGraphicsDevice/GraphicsEnvironment.h>
#include <dsGraphicsRenderer/IGraphicsRenderer.h>

GameViewport::GameViewport(const GameBinaryLoader& loader)
	: loader_(loader)
{
}

void GameViewport::SetGraphicsEnvironment(unsigned long int hwnd) const
{
	Despair::Graphics::GraphicsEnvironment environment;
	loader_.GetRenderer().GetEnvironment(environment);
	environment.m_windowHandle = hwnd;
	loader_.GetRenderer().SetEnvironment(environment);
}