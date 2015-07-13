import argparse
import cStringIO
import getpass
import glob
import os
import subprocess
import sys
import misc_helper

BUILD_DIRECTORY = os.path.dirname( os.path.join( os.getcwd(), __file__ ) )
VC_XP_VARS_BAT = os.path.join( BUILD_DIRECTORY, 'vcxpvarsall.bat' )
SRC_DIRECTORY = os.path.normpath( os.path.join( BUILD_DIRECTORY, "..", 'src') )
ROOT_DIRECTORY = os.path.join( SRC_DIRECTORY, ".." )
CMAKE_RUN_BAT = 'rerun_cmake.bat'
CMAKE_EXE = os.path.join( SRC_DIRECTORY, 'core', 'third_party', 'cmake-win32-x86', 'bin', 'cmake.exe' )
PLINK_EXE = os.path.join( SRC_DIRECTORY, 'third_party', 'putty', 'plink.exe' )
DELTACOPY_EXE = os.path.join( SRC_DIRECTORY, 'third_party', 'deltacopy', 'ssh.exe' )

DEFAULT_CONFIGS = [ 'Debug', 'Hybrid' ]

# Set up MSVC x86 environment with XP support, see
# http://blogs.msdn.com/b/vcblog/archive/2012/10/08/windows-xp-targeting-with-c-in-visual-studio-2012.aspx
VC11_X86_XP_ENV = '@call %s vc11 x86\n' % (VC_XP_VARS_BAT)
VC12_X86_XP_ENV = '@call %s vc12 x86\n' % (VC_XP_VARS_BAT)
VC11_X86_64_XP_ENV = '@call %s vc11 x64\n' % (VC_XP_VARS_BAT)
VC12_X86_64_XP_ENV = '@call %s vc12 x64\n' % (VC_XP_VARS_BAT)

CMAKE_GENERATORS = [
	dict(
		label = 'Visual Studio 2008 Win32',
		generator = 'Visual Studio 9 2008', 
		dirsuffix = 'vc9_win32',
		deprecated = True
	),
	dict(
		label = 'Visual Studio 2010 Win32',
		generator = 'Visual Studio 10', 
		dirsuffix = 'vc10_win32',
		deprecated = True,
		enableForTargets=["bwentity"]
	),
	dict(
		label = 'Visual Studio 2010 Win64',
		generator = 'Visual Studio 10 Win64', 
		dirsuffix = 'vc10_win64',
		deprecated = True,
		enableForTargets=["bwentity"]
	),
	dict(
		label = 'Visual Studio 2012 Win64 (Qt 5.3.1)',
		generator = 'Visual Studio 11 Win64', 
		dirsuffix = 'vc11_win64_qt5.3.1',
		toolset = 'v110_xp',
		qt = '5.3.1',
		experimentalForTargets=["server"]
	),
	dict(
		label = 'Visual Studio 2012 Win64 (Qt 5.4.2)',
		generator = 'Visual Studio 11 Win64', 
		dirsuffix = 'vc11_win64_qt5.4.2',
		toolset = 'v110_xp',
		qt = '5.4.2',
		experimentalForTargets=["server"]
	),
	dict(
		label = 'Visual Studio 2012 Win64 (Qt 5.5.0)',
		generator = 'Visual Studio 11 Win64',
		dirsuffix = 'vc11_win64_qt5.5.0',
		toolset = 'v110_xp',
		qt = '5.5.0',
		experimentalForTargets=["server"]
	),
	dict(
		label = 'Visual Studio 2013 Win64 (Qt 5.5.0)',
		generator = 'Visual Studio 12 Win64',
		dirsuffix = 'vc12_win64_qt5.5.0',
		toolset = 'v120_xp',
		qt = '5.5.0',
		experimentalForTargets=["server"]
	),
	dict(
		label = 'Visual Studio 2012 Win32 (Normal toolkit)',
		generator = 'Visual Studio 11', 
		dirsuffix = 'vc11_no_xp_win32',
		toolset = 'v110',
		experimental = True
	),
	dict(
		label = 'Visual Studio 2012 Win64 (Normal toolkit)',
		generator = 'Visual Studio 11 Win64', 
		dirsuffix = 'vc11_no_xp_win64',
		toolset = 'v110',
		experimental = True,
		enableForTargets=["server"]
	),
	dict(
		label = 'Visual Studio 2013 Win32',
		generator = 'Visual Studio 12', 
		dirsuffix = 'vc12_win32',
		toolset = 'v120_xp',
		experimental = True
	),
	dict(
		label = 'Visual Studio 2013 Win64',
		generator = 'Visual Studio 12 Win64', 
		dirsuffix = 'vc12_win64',
		toolset = 'v120_xp',
		experimental = True
	),
	dict(
		label = 'Ninja with MSVC11 Win32',
		generator = 'CodeBlocks - Ninja', 
		dirsuffix = 'ninja_vc11_win32',
		experimental = True,
		batchenv = VC11_X86_XP_ENV,
		singleConfig = True
	),
	dict(
		label = 'Ninja with MSVC11 Win64',
		generator = 'CodeBlocks - Ninja', 
		dirsuffix = 'ninja_vc11_win64',
		experimental = True,
		batchenv = VC11_X86_64_XP_ENV,
		singleConfig = True
	),
	dict(
		label = 'Ninja with MSVC12 Win32',
		generator = 'CodeBlocks - Ninja', 
		dirsuffix = 'ninja_vc12_win32',
		experimental = True,
		batchenv = VC12_X86_XP_ENV,
		singleConfig = True
	),
	dict(
		label = 'Ninja with MSVC12 Win64',
		generator = 'CodeBlocks - Ninja', 
		dirsuffix = 'ninja_vc12_win64',
		experimental = True,
		batchenv = VC12_X86_64_XP_ENV,
		singleConfig = True
	),
	dict(
		label = 'Visual Studio 2012 Win32 with Clang-cl',
		generator = 'Visual Studio 11', 
		dirsuffix = 'vc11_clang_win32',
		toolset = 'LLVM-vs2012_xp',
		experimental = True,
	),
	dict(
		label = 'Visual Studio 2012 Win64 with Clang-cl',
		generator = 'Visual Studio 11 Win64', 
		dirsuffix = 'vc11_clang_win64',
		toolset = 'LLVM-vs2012_xp',
		experimental = True,
	)
]

YES_NO_OPTION = [
	dict( label = 'Yes', value = True ),
	dict( label = 'No' , value = False )
]

def writeBat( contents, outputPath ):
	out = open( outputPath, 'w' )
	out.write( contents )
	out.write('\n')
	out.close()


def runBat( batFile ):
	runDir = os.path.dirname( batFile )
	process = subprocess.Popen( ['cmd.exe', '/C', batFile ],
		cwd=runDir )
	process.communicate()
	return process.returncode == 0


def targetChoices():
	def _stripName( n ):
		return n.replace( '\\', '/' ).replace( 'cmake/BWConfiguration_', '' ).replace( '.cmake', '' )
	targets = [ _stripName(x) for x in glob.glob( 'cmake/BWConfiguration_*.cmake' ) ]
	targets.sort()
	return targets


def findCMakeTargets():
	items = []
	for target in targetChoices():
		items.append( { "label" : target } )
	return items


def targetConfigs( target = None ):
	# configs to build for single config builders
	configs = DEFAULT_CONFIGS[:]
	# hardcode consumer release for client configs...
	if target == None or target == 'client' or target == 'client_blob':
		configs.append( 'Consumer_Release' )

	return configs


def generatorChoices():
	generators = []
	for generator in CMAKE_GENERATORS:
		generators.append( generator['dirsuffix'] )
	generators.sort()
	return generators


def chooseItem( prompt, items, deprecated = False, experimental = False, targets = None ):

	# filter out items we aren't interested in
	def displayItem( item ):
		if targets:
			for target in targets:
				if target in item.get( 'enableForTargets', [] ):
					return True
				if target in item.get( 'experimentalForTargets', [] ) and not experimental:
					return False
		if item.get( 'deprecated', False ) and not deprecated:
			return False
		if item.get( 'experimental', False ) and not experimental:
			return False
		return True
	items = filter(displayItem, items)

	if len(items) == 1:
		return items[0]

	print prompt
	for i in range( len(items) ):
		item = items[i]
		label = item['label']
		print '\t%d. %s' % (i + 1, label)
	choice = None
	while choice is None:
		try:
			choice = int(raw_input('> '))
			choice -= 1
			if choice < 0 or choice >= len( items ):
				choice = None
		except ValueError:
			choice = None
		
		if choice is None:
			print "Invalid option"
	return items[choice]


def editableList( prompt, items ):
	while True:
		print prompt
		for i in range( len(items) ):
			item = items[i]
			label = item[0]
			value = item[1]
			if type( value ) is tuple:
				value = value[0]
			print "\t%d. %s = %s" % (i+1, label, value)
		choice = None

		try:
			choice = int(raw_input('> '))
			choice -= 1
			if choice < 0 or choice >= len(items):
				choice = None
		except ValueError:
			choice = None

		if choice is None:
			break

		if len(items[choice]) == 2:
			items[choice][1] = raw_input(items[choice][0] + "> ")
		else:
			items[choice][1] = chooseItem(items[choice][0], items[choice][2])


def testPlinkConnection( username, hostname, privateKeyPath, linuxPath ):
	hasValidConfig = False
	try:
		plinkArgs = [PLINK_EXE, '-batch', '-v']
		if privateKeyPath == "":
			plinkArgs.append( "-agent" )
		else:
			plinkArgs += ['-i', '%s' % (privateKeyPath)]
		plinkArgs.append( r'%s@%s' % (username, hostname) )
		plinkArgs += ['if [[ -f %s/CMakeLists.txt && -f %s/Makefile && -d %s/build ]]; then echo CMAKE_SUCCESS; else echo CMAKE_BAD_PATH; fi' % ( linuxPath, linuxPath, linuxPath )]
		print plinkArgs
		plink = subprocess.Popen( plinkArgs, cwd=BUILD_DIRECTORY,
				universal_newlines=True, stdin=subprocess.PIPE,
				stdout=subprocess.PIPE, stderr=subprocess.PIPE )
		# We use stdin so plink doesn't steal our stdin
		plink.stdin.close()
		plink.wait()
		stdout = plink.stdout.read()
		stderr = plink.stderr.read()
		plink.stdout.close()
		plink.stderr.close()
		if stdout.strip() == "CMAKE_SUCCESS":
			hasValidConfig = True
			print "Managed to establish connection"
		elif stdout.strip() == "CMAKE_BAD_PATH":
			print "Incorrect mount path"
		else:
			print stderr
	except OSError, e:
		print "Unable to execute plink %s" % str(e)
	return hasValidConfig


def testDeltaCopyConnection( username, hostname, privateKeyPath, linuxPath ):
	hasValidConfig = False
	try:
		deltaCopy = subprocess.Popen( 
			[
				DELTACOPY_EXE, '-oUserKnownHostsFile=/dev/null',
				'-oStrictHostKeyChecking=no', '-oBatchMode=yes',
				'-i', '%s' % (privateKeyPath), '%s@%s' % (username, hostname), 
				'if [[ -f %s/CMakeLists.txt ]]; then echo CMAKE_SUCCESS; else echo CMAKE_BAD_PATH; fi' % linuxPath 
			],
			universal_newlines=True, cwd=BUILD_DIRECTORY,
			stdin=subprocess.PIPE, stdout=subprocess.PIPE,
			stderr=subprocess.PIPE)
		# We use stdin so plink doesn't steal our stdin
		deltaCopy.stdin.close()
		deltaCopy.wait()
		stdout = deltaCopy.stdout.read()
		stderr = deltaCopy.stderr.read()
		deltaCopy.stdout.close()
		deltaCopy.stderr.close()
		if stdout.strip() == "CMAKE_SUCCESS":
			hasValidConfig = True
			print "Managed to establish connection"
		elif stdout.strip() == "CMAKE_BAD_PATH":
			print "Incorrect mount path"
		else:
			print stderr
	except OSError, e:
		print "Unable to execute deltacopy %s" % str(e)
	return hasValidConfig


def getServerOptions(allowRsync = False):
	hostname = raw_input('Hostname> ')
	server_directory = raw_input('Linux "programming/bigworld" path> ')

	CONNECTION_TYPES = [
		dict( label = "Putty SSH Session to Windows Mount", value = "PUTTY_SSH" ),
		dict( label = "DeltaCopy SSH Session to Windows Mount",	value = "DELTACOPY_SSH" ),
		]
	
	if allowRsync:
		CONNECTION_TYPES.append( 
			dict( label = "DeltaCopy rsync to Linux copy", value = "DELTACOPY_RSYNC" ) )

	connection_type = chooseItem( "How do you wish to connect to the server?", 
			CONNECTION_TYPES )

	private_key_path = raw_input('Private key path if needed> ')

	hasValidConfig = False

	serverOptions = [
		["Connection type", connection_type["value"], CONNECTION_TYPES],
		["Username", getpass.getuser()],
		["Hostname", hostname],
		["Private key path", private_key_path],
		["Linux source path", server_directory],
		["Compile Flags", ""],
		["Replace Paths in Output", "ON", ["ON","OFF"]]
	]
	
	serverOptionsKeys = {
		'connectionType': 0,
		'username': 1,
		'hostname': 2,
		'privateKeyPath': 3,
		'linuxPath': 4,
		'compileFlags': 5,
		'replacePathsInOutput': 6,
		}
		
	def opt(key):
		return serverOptions[serverOptionsKeys[key]][1]
	
	while not hasValidConfig:
		editableList("Server Options (Leave Blank to continue)", serverOptions)
		
		if opt('connectionType') == "PUTTY_SSH":
			print "Testing PuTTY connection"
			hasValidConfig = testPlinkConnection( opt('username'),
				opt('hostname'), opt('privateKeyPath'), opt('linuxPath') )
		else:
			print "Testing DeltaCopy connection"
			hasValidConfig = testDeltaCopyConnection( opt('username'),
				opt('hostname'), opt('privateKeyPath'), opt('linuxPath') )

	return [
				'-DBW_LINUX_CONN_TYPE="%s"' % (opt('connectionType')),
				'-DBW_LINUX_USER="%s"' % (opt('username')),
				'-DBW_LINUX_HOST="%s"' % (opt('hostname')),
				'-DBW_LINUX_PRIV_KEY="%s"' % (opt('privateKeyPath')),
				'-DBW_LINUX_DIR="%s"' % (opt('linuxPath')),
				'-DBW_LINUX_CFLAGS="%s"' % (opt('compileFlags')),
				'-DBW_REPLACE_LINUX_DIR="%s"' % (opt('replacePathsInOutput'))
			]


def serverOpts( targetName, generator, args ):
	# check for server builds
	opts = [ '-DBW_VERIFY_LINUX=OFF' ]
	if targetName == 'server':
		verifyServer = chooseItem(
				'Would you like to build remotely?',
				YES_NO_OPTION )
		if verifyServer['value']:
			generator['dirsuffix'] = 'linux'
			opts = getServerOptions(False)
	elif args.show_verify_server:
		verifyServer = chooseItem(
				'Would you like to add server compile pre-build steps?',
				YES_NO_OPTION )
		if verifyServer['value']:
			opts = getServerOptions(True)
			opts.append( '-DBW_VERIFY_LINUX=ON' )
	return opts


def chooseMayaVersion():
	MAYA_VERSIONS = [
		#dict( label = 'Maya 2012', version = '2012' ),
		#dict( label = 'Maya 2013', version = '2013' ),
		dict( label = 'Maya 2014', version = '2014' ),
		dict( label = 'Maya 2015', version = '2015' ),
		dict( label = 'Maya 2016', version = '2016' ),
	]
	return chooseItem( "Which Maya version you want to build with ?", MAYA_VERSIONS )['version']


def buildDir( targetName, generator, buildRoot ):
	path = os.path.normpath( os.path.join( buildRoot,
		('build_%s_%s' % (targetName, generator['dirsuffix'])).lower() ) )
	if 'maya' in generator:
		path = os.path.join( path, generator['maya'] )
	return path



def writeGenerateBat( targetName, generator, cmakeExe, cmakeOpts, buildRoot, dryRun ):
	# create output directory
	outputDir = buildDir( targetName, generator, buildRoot )

	if not dryRun and not os.path.isdir( outputDir ):
		os.makedirs( outputDir )

	# generate cmake command list
	cmd = [
		cmakeExe, '"%s"' % SRC_DIRECTORY,
		'-Wno-dev', # disable CMakeLists.txt developer warnings
		'-G"%s"' % generator['generator'],
		'-DBW_CMAKE_TARGET=%s' % targetName,
 		'-DQT_VERSION=%s' % generator['qt']
	]

	# check for asset compiler allowed hosts
	if misc_helper.isHostnameAllowed():
		cmd.append('-DBW_ASSET_COMPILER_OPTIONS_ENABLE_CACHE=ON')

	# optionally append maya version
	if 'maya' in generator:
		cmd.append( '-DMAYA_VERSION=%s' % generator['maya'] )

	# optionally append toolset
	if ('toolset' in generator):
		cmd.append( '-T' )
		cmd.append( generator['toolset'] )

	# append server build options
	if cmakeOpts:
		cmd = cmd + cmakeOpts

	# for single config builders (make/ninja) we need a list of configs to generate
	if generator.get('singleConfig', False):
		configs = targetConfigs( targetName )
	else:
		configs = []

	# output out command
	cmdstr = ' '.join( cmd )

	# write and execute the cmake run bat file
	outputPath = os.path.join( outputDir, CMAKE_RUN_BAT )
	out = cStringIO.StringIO()

	batchenv = generator.get('batchenv', '')
	if batchenv:
		out.write( batchenv )

	out.write( '@pushd %~dp0\n' )

	if configs:
		# if single config builder then create subdirs for each config
		for config in configs:
			mkdir = '@if not exist %s\\nul mkdir %s\n' % ( config, config )
			out.write( mkdir )

		for config in configs:
			out.write( 'pushd %s\n' % ( config ) )
			out.write( '%s -DCMAKE_BUILD_TYPE=%s\n' % ( cmdstr, config ) )
			out.write( 'popd\n' )
	else:
		# otherwise write out single command
		out.write( cmdstr )

	out.write('\n@popd\n')

	print 'writing bat> ', outputPath
	if not dryRun:
		writeBat( out.getvalue(), outputPath )
	out.close()
	return outputPath


def writeBuildBat( targetName, config, generator, cmakeExe, buildRoot, rebuild, dryRun ):
	outputDir = buildDir( targetName, generator, buildRoot )

	assert os.path.isdir( outputDir ), "output directory doesn't exist"

	batchenv = generator.get('batchenv', '')

	if generator.get( 'singleConfig', False ):
		buildCmd = [ cmakeExe, '--build', '"%s"' % os.path.join( outputDir, config ) ]
		rebuildCmd = [ cmakeExe, '--build', '"%s"' % os.path.join( outputDir, config ), '--clean-first' ]
	else:
		buildCmd = [ cmakeExe, '--build', '"%s"' % outputDir, '--config', config ]
		rebuildCmd = [ cmakeExe, '--build', '"%s"' % outputDir, '--config', config, '--clean-first' ]

	buildCmdStr = ' '.join( buildCmd )
	rebuildCmdStr = ' '.join( rebuildCmd )

	buildBatFile = 'build_%s.bat' % config.lower()
	buildBatPath = os.path.join( outputDir, buildBatFile )

	rebuildBatFile = 'rebuild_%s.bat' % config.lower()
	rebuildBatPath = os.path.join( outputDir, rebuildBatFile )
	
	def _writeBuildBat( outputPath, cmdstr ):
		out = cStringIO.StringIO()

		if batchenv:
			out.write( batchenv )

		out.write( cmdstr )
		out.write('\n')
		print 'writing bat> ', outputPath
		if not dryRun:
			writeBat( out.getvalue(), outputPath )
		out.close()

	_writeBuildBat( buildBatPath, buildCmdStr )
	_writeBuildBat( rebuildBatPath, rebuildCmdStr )

	# return the batch file that should be run, depending on build or rebuild option
	if rebuild:
		return rebuildBatPath
	else:
		return buildBatPath


def main():
	# parse command line options
	parser = argparse.ArgumentParser()
	parser.add_argument( 'builddir', nargs='?', type=str,
			default=ROOT_DIRECTORY, help='set root build directory' )
	parser.add_argument( '--target', action='append',
			choices=targetChoices(),
			help='generate the specified targets' )
	parser.add_argument( '--generator', action='append',
			choices=generatorChoices(),
			help='use the specified generators' )
	parser.add_argument( '--build', action='store_true',
			help='build the generated targets' )
	parser.add_argument( '--rebuild', action='store_true',
			help='rebuild the generated targets' )
	parser.add_argument( '--config', action='append',
			choices=targetConfigs(),
			help='build the specified configs' )
	parser.add_argument( '--experimental', action='store_true',
			help='show experimental options' )
	parser.add_argument( '--deprecated', action='store_true',
			help='show deprecated options' )
	parser.add_argument( '--cmake-exe', type=str,
			help='specify a CMake exe to use' )
	parser.add_argument( '--show-verify-server', action='store_true',
			help='show server build verification option' )
	parser.add_argument( '--dry-run', action='store_true',
			help='query build options but don\'t run CMake')
	args = parser.parse_args()

	targets = []
	generators = []

	# choose target project
	if args.target is None:
		targetItems = findCMakeTargets()
		target = chooseItem( "What do you want to build?", targetItems )
		targets = [ target['label'] ]
	else:
		targets = args.target

	# choose generator
	if args.generator is None:
		generators = [ chooseItem( "What do you want to build with?",
				CMAKE_GENERATORS, args.deprecated, args.experimental,
				targets = targets ) ]
	else:
		for generator in args.generator:
			for cmake_generator in CMAKE_GENERATORS:
				if cmake_generator['dirsuffix'] == generator:
					generators.append( cmake_generator )
		assert( len(generators) != 0 )
	
	# set cmake executable
	if args.cmake_exe:
		cmakeExe = os.path.normpath( args.cmake_exe )
	else:
		cmakeExe = CMAKE_EXE
	cmakeExe = '"%s"' % ( cmakeExe, )

	# lists of batch files to run for generation and build steps
	generateBats = []
	buildBats = []

	# write batch files
	for generator in generators:
		for target in targets:
			# optionally append maya version
			if target == 'maya_plugin':
				generator[ 'maya' ] = chooseMayaVersion()

			cmakeOpts = serverOpts( target, generator, args )
			genBat = writeGenerateBat( target, generator, cmakeExe, cmakeOpts,
					args.builddir, args.dry_run );
			generateBats.append( genBat )
			configs = targetConfigs( target )
			for config in configs:
				buildBat = writeBuildBat( target, config, generator, cmakeExe,
						args.builddir, args.rebuild, args.dry_run )
				# only append batch file if we want to build
				if (args.build or args.rebuild) and (args.config is None or config in args.config):
					buildBats.append( buildBat )
	
	# run all generated batch files
	for bat in generateBats:
		print 'running bat>', bat
		if not args.dry_run:
			runBat( bat )
	for bat in buildBats:
		print 'running bat>', bat
		if not args.dry_run:
			runBat( bat )


if __name__ == '__main__':
	main()

