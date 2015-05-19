#ifndef _COOPERATIVE_MOO_HPP_
#define _COOPERATIVE_MOO_HPP_

namespace BW
{

class CooperativeMoo
{
public:
	static bool beginOnPaint( IEditorApp * editorApp ) { return true; }
	static void endOnPaint( IEditorApp * editorApp ) { }
};

}

#endif _COOPERATIVE_MOO_HPP_
