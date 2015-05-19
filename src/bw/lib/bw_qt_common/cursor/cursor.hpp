#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <functional>

class Cursor
{
public:
	typedef std::function< void ( int, int ) >	CursorExplicitlyChanged;

	static void setPosition( int x, int y );
	static void addPositionChangedListener( CursorExplicitlyChanged & listener );

private:
	static void emitCursorPosChanged( int x, int y );
};

#endif //CURSOR_HPP