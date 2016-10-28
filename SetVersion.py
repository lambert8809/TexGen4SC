import re, sys, getopt
# import difflib

def ModifySourceCode(SourceFileName, NewVersion):
	OldVersion = [0,0,0]
	TGSource = open(SourceFileName, 'r').read()
	# OriginalSource = TGSource
	for i in range(3):
		if i == 0:
			pattern = r'(, m_iMajorVersion)\(([0-9]*)\)'
		if i == 1:
			pattern = r'(, m_iMinorVersion)\(([0-9]*)\)'
		if i == 2:
			pattern = r'(, m_iRevision)\(([0-9]*)\)'
		Result = re.findall(pattern, TGSource)
		assert(len(Result) == 1)
		OldVersion[i] = int(Result[0][1])
		replace = r'\1(%d)' % NewVersion[i]
		TGSource, NumChanges = re.subn(pattern, replace, TGSource)
		assert(NumChanges == 1)

	# diffList = list(difflib.unified_diff(OriginalSource.splitlines(True), TGSource.splitlines(True), n=0))
	# sys.stdout.writelines(diffList)

	OutputFile = open(SourceFileName, 'w')
	OutputFile.write(TGSource)
	
	return OldVersion

def ModifyInstaller(FileName, NewVersion):
	OldVersion = [0,0,0]
	Source = open(FileName, 'r').read()
	# OriginalSource = TGSource

	pattern = r'!define VERSION "([0-9]*).([0-9]*).([0-9]*)"'
	Result = re.findall(pattern, Source)
	assert(len(Result) == 1)
	OldVersion = [int(x) for x in Result[0]]
	replace = r'!define VERSION "%d.%d.%d"' % tuple(NewVersion)
	Source, NumChanges = re.subn(pattern, replace, Source)
	assert(NumChanges == 1)

	# diffList = list(difflib.unified_diff(OriginalSource.splitlines(True), TGSource.splitlines(True), n=0))
	# sys.stdout.writelines(diffList)

	OutputFile = open(FileName, 'w')
	OutputFile.write(Source)
	
	return OldVersion

def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:], "")
		NewVersion = [float(x) for x in args]
		assert(len(NewVersion) == 3)
	except:
		print 'Usage: python SetVersion.py <major> <minor> <revision>'
		return

	FileNames = ['Core/TexGen.cpp', 'Installer/texgen.nsi', 'Installer/texgen-bundle.nsi', 'Installer/texgen2008.nsi', 'Installer/texgen-bundle2008.nsi', 'Installer/texgen-64bit-bundle2008.nsi','Installer/texgen-bundle2008_Python27.nsi', 'Installer/texgen-64bit-bundle-Python27-2008.nsi']
	NumFiles = len(FileNames)
	print('Length filenames = ', NumFiles )
	OldVersions = [None] * NumFiles
	
	OldVersions[0] = ModifySourceCode(FileNames[0], NewVersion)
	
	#OldVersions = [None] * 6
	for i in range(1, NumFiles ):
		OldVersions[i] = ModifyInstaller(FileNames[i], NewVersion)
		print( 'Modified OldVersions', i)
		
	
	#OldVersions[1] = ModifyInstaller(FileNames[1], NewVersion)
	#OldVersions[2] = ModifyInstaller(FileNames[2], NewVersion)
	#OldVersions[3] = ModifyInstaller(FileNames[3], NewVersion)
	#OldVersions[4] = ModifyInstaller(FileNames[4], NewVersion)
	#OldVersions[5] = ModifyInstaller(FileNames[5], NewVersion)

	OldVersion = OldVersions[0]
	
	Conflict = False
	for v in OldVersions:
		if v != OldVersion:
			Conflict = True
	
	if Conflict:
		print 'Version conflict between files:'
		for FileName, OldVersion in zip(FileNames, OldVersions):
			print 'Changed %s from %d.%d.%d to %d.%d.%d' % tuple([FileName] + OldVersion + NewVersion)
	else:
		print 'Changed TexGen from version %d.%d.%d to version %d.%d.%d' % tuple(OldVersion + NewVersion)


if __name__ == '__main__':
	main()
