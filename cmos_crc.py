import sys

cksum = 0
i = 0
for line in open("/tmp/x", "r"):
	data = line.strip().split(' ')
	print(data)
	print ("i=", i)
	for d in data:
		print(d,cksum)
		cksum = cksum + (int("0x" + d,16))
		cksum = 0xff & cksum
		i = i + 1

cksum = ~cksum
cksum = cksum & 0xff
print (i)
print(("%x")%(cksum))
