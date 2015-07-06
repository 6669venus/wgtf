//--------------------------------------------------------------------------------------------------
//
// maya_plugin.hpp
//
//--------------------------------------------------------------------------------------------------
//  Copyright (c) 2015 Wargaming World, Ltd. All rights reserved.
//--------------------------------------------------------------------------------------------------

#pragma warning( push )
#pragma warning( disable: 4244 4100 4238 4239 4263 4245 4201 )

#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MTemplateCommand.h>

#include <vector>

class NGTEventLoop;
class MayaWindow;
class GenericPluginManager;
class NGTMayaPlugin;
char cmdName[] = "NGTMaya";

class NGTMayaPlugin : public MTemplateAction< NGTMayaPlugin, cmdName, MTemplateCommand_nullSyntax >
{
public:
	NGTMayaPlugin();
	virtual ~NGTMayaPlugin();

	MStatus doIt(const MArgList& args) override;

private:
	bool loadNGT( const MArgList& args );
	bool getNGTPlugins(std::vector< std::wstring >& plugins, const wchar_t* filepath);	
	NGTEventLoop * ngtEventLoop_;
	MayaWindow * mayaWindow_;
	bool ngtLoaded_;
	GenericPluginManager * pluginManager_;
};
