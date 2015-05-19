import os

TEST_ITEMS = (
	{
		"target" : "generic_app",
		"name" : "Generic App Model Editor (win64)",
		"bin" : os.path.join( "tools", "generic_app" ),
		"exe" : "generic_app.exe",
		"build config" : "win64",
		"copy space settings" : False,
		"options" : "",
		"script" : "",
		"settings" : "",
		"plugins" : "plugins.txt",
		"preferences" : "",
		"ignore leaks" : True,
		"known issue" : "",
		"args" : "",
		"timeout" : 600
	},
	{
		"target" : "generic_app",
		"name" : "Generic App World Viewer (win64)",
		"bin" : os.path.join( "tools", "generic_app" ),
		"exe" : "generic_app.exe",
		"build config" : "win64",
		"copy space settings" : False,
		"options" : "",
		"script" : "",
		"settings" : "",
		"plugins" : "world_viewer_plugins.txt",
		"preferences" : "",
		"ignore leaks" : True,
		"known issue" : "",
		"args" : "",
		"timeout" : 600
	}
)

