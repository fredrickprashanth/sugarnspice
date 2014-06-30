
target_wt = 2
x = (0, 0, 0, 0, 0, 0)
w = (1, 0.5, 0.2, 0.05, 0.02, 0.01)
ii = []
search_soln = set([])
search_soln |= set([x])

def weight(x):
	wt = 0
	for i in range(0, len(x)):
		wt = wt + w[i]*x[i]
	return wt
		
def mat_add(x,y):
	z = []
	for i in range(0, len(x)):
		z.append(x[i] + y[i])
	return tuple(z)
	
for i in range(0, len(x)):
	jj = [0, 0, 0, 0, 0, 0]
	jj[i] = 1
	ii.append(tuple(jj))


soln = set([])
while len(search_soln) > 0:
	temp_search_soln = search_soln.copy()
	search_soln.clear()
	for s in temp_search_soln:
		for jj in ii:
			kk = mat_add(s, jj)
			if weight(kk) == target_wt:
				#print("Found soln", kk)
				soln |= set([kk])
			elif weight(kk) < target_wt:
				search_soln |= set([kk])
	print("Len of soln = ", len(search_soln))

print ("Total solutions =", len(soln))
		









