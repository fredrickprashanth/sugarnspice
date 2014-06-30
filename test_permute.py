import copy
def _swap(x, i, j):
	if i == j: return
	t = x[i]
	x[i] = x[j]
	x[j] = t

def _permute(x, start, end):
	#print("new frame start =", start, " end =", end)
	if end == start:
		print(x)
	y = copy.deepcopy(x)
	for i in range(0, end - start+1):
		#print("start =", start, "end =", end, "swapping at ", start, "to ", i+start)
		_swap(y, start, start+i)
		_permute(y, start+1, end)
	



def _permute_lex(x):
	found = False
	for i in range(len(x)-1, 0, -1):
		if ord(x[i]) > ord(x[i-1]):
			found = True
			break
	if found == False:
		return False
	
	k = i-1
	#print("Found k at ", k, x[k])
	for i in range(len(x)-1, k, -1):
		if ord(x[i]) > ord(x[k]):
			break
	l = i
	#print("Found l at ", l, x[l])

	_swap(x, k, l)
	start = k + 1
	end = len(x) - 1
	while start < end:
		#print("Swap ", x[start], x[end])
		_swap(x, start, end)
		start = start + 1
		end = end - 1
	return True
			

def permute(x):
	x_l = []
	for c in x:
		x_l.append(c)
	#_permute(x_l, 0, len(x_l)-1)
	print(x_l)
	while _permute_lex(x_l):
		print(x_l)





sample_str="abcd"

permute(sample_str)
