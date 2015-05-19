import os

TEST_ITEMS = (
	{
		"target" : "modeleditor",
		"name" : "Model Editor (win64)",
		"bin" : os.path.join( "tools", "modeleditor" ),
		"exe" : "modeleditor.exe",
		"build config" : "win64",
		"copy space settings" : False,
		"options" : "modeleditor.options",
		"script" : "modeleditorsmoketest",
		"settings" : "",
		"plugins" : "",
		"preferences" : "",
		"ignore leaks" : True,
		"known issue" : "",
		"args" : "",
		"timeout" : 600
	},
)

