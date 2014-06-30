input = [0, 1, 10, 13, 19, 21, 34]
look = 10

def bsearch(list, key):
	l = 0
	r = len(list)-1
	while l <= r:
		m = l + (r - l)/2
		if list[m] == key:
			return m
		elif list[m] > key:
			r = m - 1
		else:
			l = m + 1
	
	return -1

print "input = ", input
print "bsearch of ", look, " = ", bsearch(input, look)
