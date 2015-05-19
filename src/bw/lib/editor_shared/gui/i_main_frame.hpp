#ifndef I_MAIN_FRAME_H
#define I_MAIN_FRAME_H

class IMainFrame
{
public:
	virtual ~IMainFrame() {}

	virtual void setMessageText( const wchar_t * pText ) = 0;
	virtual void setStatusText( unsigned int id, const wchar_t * text ) = 0;
	virtual bool isActiveWindow() const { return false;}

	virtual void updateGUI( bool force = false ) = 0;

	virtual void grabFocus() = 0;

};

#endif //I_MAIN_FRAME_H