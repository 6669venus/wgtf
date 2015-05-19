#include "cursor.hpp"

#include <vector>
#include "cstdmf/singleton_manager.hpp"

#include <QCursor>

namespace BW
{
	class CursorListeners
		: public std::vector< Cursor::CursorExplicitlyChanged * >
	{
	public:
		CursorListeners()
		{
			REGISTER_SINGLETON( CursorListeners )
		}

		static CursorListeners & instance();
	};

	CursorListeners s_PosChangedListeners;

	CursorListeners & CursorListeners::instance()
	{
		SINGLETON_MANAGER_WRAPPER( CursorListeners );
		return s_PosChangedListeners;
	}
}

//==============================================================================
/*static */void Cursor::addPositionChangedListener(
	CursorExplicitlyChanged & listener )
{
	BW::CursorListeners::instance().push_back( &listener );
}


//==============================================================================
/*static */void Cursor::emitCursorPosChanged( int x, int y )
{
	BW::CursorListeners & listeners = BW::CursorListeners::instance();
	for( CursorExplicitlyChanged * handler : listeners )
	{
		(*handler)( x, y );
	}
}


//==============================================================================
/*static */void Cursor::setPosition( int x, int y )
{
	QCursor::setPos( x, y );
	emitCursorPosChanged( x, y );
}


