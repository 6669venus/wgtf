import os

TEST_ITEMS = (
	{
		"target" : "batch_compiler",
		"name" : "Batch Compiler (win64)",
		"bin" : os.path.join( "tools", "asset_pipeline" ),
		"exe" : "batch_compiler.exe",
		"build config" : "win64",
		"copy space settings" : False,
		"options" : "",
		"script" : "",
		"settings" : "",
		"plugins" : "",
		"preferences" : "",
		"ignore leaks" : True,
		"known issue" : "",
		"args" : "-cachePath \\\\ASSETCACHE\\assetcache\\qt_tools_staging -j 8 -report report.html",
		"timeout" : 3600
	},
)

