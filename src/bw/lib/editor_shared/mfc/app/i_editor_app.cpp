#include "i_editor_app.hpp"
#include <afxwin.h>

//==============================================================================
bool IEditorApp::isMinimized()
{
	return AfxGetMainWnd()->IsIconic() == TRUE;
}
