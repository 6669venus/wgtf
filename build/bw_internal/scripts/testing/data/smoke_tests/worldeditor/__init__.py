import os

TEST_ITEMS = (
	{
		"target" : "worldeditor",
		"name" : "World Editor (win64)",
		"bin" : os.path.join( "tools", "worldeditor" ),
		"exe" : "worldeditor.exe",
		"build config" : "win64",
		"copy space settings" : False,
		"options" : "worldeditor.options",
		"script" : "worldeditorsmoketest",
		"settings" : "",
		"plugins" : "",
		"preferences" : "",
		"ignore leaks" : True,
		"known issue" : "",
		"args" : "",
		"timeout" : 600
	},
)

