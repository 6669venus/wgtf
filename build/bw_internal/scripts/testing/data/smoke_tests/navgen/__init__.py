import os

TEST_ITEMS = (
	{
		"target" : "navgen",
		"name" : "Navgen (win64)",
		"bin" : os.path.join( "tools", "navgen" ),
		"exe" : "navgen.exe",
		"build config" : "win64",
		"copy space settings" : False,
		"options" : "",
		"script" : "navgensmoketest",
		"settings" : "navgen_settings.xml",
		"plugins" : "",
		"preferences" : "",
		"ignore leaks" : True,
		"known issue" : "",
		"args" : "",
		"timeout" : 600
	},
)

