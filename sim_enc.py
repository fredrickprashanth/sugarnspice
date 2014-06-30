import sys
import string

tr_from ="abcdefghijklmnopqrstuvwxyz"
tr_to =  "bcdefghijklmnopqrstuvwxyza"
tr_table = string.maketrans(tr_from, tr_to)
f = open(sys.argv[1])
for l in f:
	l = l.strip()
	print l.translate(tr_table)
		
f.close()
	
