"""
just curious to know how much native code there is in the project...
"""

import os

def getLineCount(filename):
	f = open(filename)
	result = len(f.read().split('\n'))
	f.close()
	return result
	
def getFiles(folderPath):
	l = os.listdir(folderPath)
	ignoreList = ['Output', 'Makefile', 'RLP.h', 'lpc23xx.h']
	ignoreTypes = ['.i', '.s', '.lds']
	result = []
	for f in l:

		if f in ignoreList:
			continue
			
		add = True			
		for i in ignoreTypes:
			if i in f:
				add = False
				break
				
		if add:
			result.append(f)
		
	return result
	
def main():
	folder = 'Native'
	files = getFiles(folder)
	print '%s files' % len(files)
	total = 0
	for file in files:
		lines = getLineCount(folder + os.sep + file)
		print '%s : %s' % (file, lines)
		total += lines
		
	print '\ntotal: %s' % total
	
if __name__ == "__main__":
	main()