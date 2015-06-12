#ifndef BW_MESSAGE_BOX_HPP
#define BW_MESSAGE_BOX_HPP

enum MessageBoxFlags
{
	BW_MB_OK				= 1,
	BW_MB_ICONWARNING		= 1 << 1,
	BW_MB_ICONEXCLAMATION	= 1 << 2
};

int CreateMessageBox( void * parent, const wchar_t * text, const wchar_t * caption,
				MessageBoxFlags flags );

#endif // BW_MESSAGE_BOX_HPP
