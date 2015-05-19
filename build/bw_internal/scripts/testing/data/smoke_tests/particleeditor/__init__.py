import os

TEST_ITEMS = (
	{
		"target" : "particleeditor",
		"name" : "Particle Editor (win64)",
		"bin" : os.path.join( "tools", "particleeditor" ),
		"exe" : "particleeditor.exe",
		"build config" : "win64",
		"copy space settings" : False,
		"options" : "particleeditor.options",
		"script" : "particleeditorsmoketest",
		"settings" : "",
		"plugins" : "",
		"preferences" : "",
		"ignore leaks" : True,
		"known issue" : "",
		"args" : "",
		"timeout" : 600
	},
)

