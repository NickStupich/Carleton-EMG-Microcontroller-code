def getSectionSpace(text, name, name2 = None):
	try:
		ind = text.index(name)
	except Exception,e:
		if name2 is not None:
			ind= text.index(name2)
	#print ind
	#return 1
	
	return int(text[ind:].split()[2], 16)

f = open('Native/Output/RLP_test.map')
content = f.read()
f.close()

available = int('27fc', 16)

text = getSectionSpace(content, '.text')
rodata = getSectionSpace(content, '.rodata.str1.4', '.rodata')
data = getSectionSpace(content, '.data')
bss = getSectionSpace(content, '.bss')

used = text + rodata + data + bss

print 'used %s out of %s - %s full' % (used, available, 100.0 * used / available)
print 'text: %s' % text
print 'rodata: %s' % rodata
print 'data: %s' % data
print 'bss: %s' % bss