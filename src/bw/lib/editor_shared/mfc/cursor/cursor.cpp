#include "cursor.hpp"

#include "cstdmf/cstdmf_windows.hpp"

void Cursor::addPositionChangedListener( CursorExplicitlyChanged & listener )
{

}

//==============================================================================
/*static */void Cursor::emitCursorPosChanged( int x, int y )
{
}

//==============================================================================
/*static */void Cursor::setPosition( int x, int y )
{
	::SetCursorPos( x, y );
	emitCursorPosChanged( x, y );
}


