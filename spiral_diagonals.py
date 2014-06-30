

nr_side = 7

spiral=[]
for i in range(0, nr_side):
	curr_row=[]
	for j in range(0, nr_side):
		curr_row.append(0)
	spiral.append(curr_row)

r=int((nr_side-1)/2)
c=int((nr_side-1)/2) 

count = 1
#0 is  up
#1 is right
#2 is down
#3 is left
direction = 0

def next_move(spiral, curr_pos):
	r,c,direction = curr_pos
	if direction == 0 and spiral[r][c+1] != 0:
		r = r - 1
	elif direction == 1 and spiral[r+1][c] != 0:
		c = c + 1
	elif direction == 2 and spiral[r][c-1] != 0:
		r = r + 1
	elif direction == 3 and spiral[r-1][c] != 0:
		c = c - 1
	else:
		direction = (direction + 1)%4
		if direction == 0:
			r = r - 1
		elif direction == 1:
			c = c + 1
		elif direction == 2:
			r = r + 1
		elif direction == 3:
			c = c - 1
	return (r,c,direction)
	
while r < nr_side and c < nr_side:
	#print(("row,col(%d,%d)=%d dir=%d")%(r,c, count, direction))
	spiral[r][c] = count
	r,c,direction = next_move(spiral, (r,c,direction))
	#print("next r=", r, "c=", c, "dir=", direction)
	count = count + 1


i = 0
j = nr_side - 1
diag_sum = 0
while i < nr_side and j >= 0:
	diag_sum = diag_sum + spiral[i][i]
	if i != j:
		diag_sum = diag_sum + spiral[i][j]
	i = i + 1
	j = j - 1
	
	
for i in range(0, nr_side):
	for j in range(0, nr_side):
		print(("%4d")%spiral[i][j],end='')
	print("")

print("Diagnal sum =", diag_sum)
		
