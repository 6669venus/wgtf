#ifndef RENDER_CONTEXT_HPP
#define RENDER_CONTEXT_HPP

typedef unsigned __int32	uint32;

namespace BW
{

namespace Moo
{

class RenderContext
{
public:
	bool changeMode( uint32 modeIndex, bool windowed, bool testCooperative = false, uint32 backBufferWidthOverride = 0 ) { return true; }
	uint32 modeIndex( ) const { return 0u; }
	bool windowed( ) const { return true; }
};

RenderContext& rc() { static RenderContext s_rc; return s_rc; }

}

}

#endif // RENDER_CONTEXT_HPP
