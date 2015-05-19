#ifndef INPUT_HPP
#define INPUT_HPP

typedef unsigned __int64 WPARAM;

namespace BW
{

namespace KeyCode
{
	enum Key
	{
		KEY_NONE			= 0x00,
		KEY_LEFTMOUSE       = 0x100,
		KEY_RIGHTMOUSE      = 0x101,
		KEY_MIDDLEMOUSE     = 0x102
	};
}

// Dummy implementation
class InputDevices
{
public:
	static InputDevices& instance() { static InputDevices s_input; return s_input; }

	void pushMouseButtonEvent( KeyCode::Key key, bool isDown, const Vector2& cursorPos) { }
	void pushMouseEvent( long dx, long dy, long dz, const Vector2& cursorPos, bool accumulate=true ) { }

	void pushKeyEvent( KeyCode::Key key,  bool isDown, const Vector2 & cursorPos ) { }

	KeyCode::Key getKeyCodeUp( WPARAM& wParam, bool extendedBit ) { return KeyCode::KEY_NONE; }
	KeyCode::Key getKeyCodeDown( WPARAM& wParam, bool extendedBit ) { return KeyCode::KEY_NONE; }
};

}

#endif
