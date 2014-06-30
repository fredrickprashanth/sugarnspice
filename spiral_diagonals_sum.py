
spiral_len = 1001
def spiral_top_right_corner(n):
	return (n*n)
def spiral_top_left_corner(n):
	return (n*n -(n-1))
def spiral_bottom_left_corner(n):
	return (n*n -2*(n-1))
def spiral_bottom_right_corner(n):
	return (n*n -3*(n-1))

diag_sum = 1
for i in range(3, spiral_len+2, 2):
	diag_sum = diag_sum + spiral_top_right_corner(i)
	diag_sum = diag_sum + spiral_top_left_corner(i)
	diag_sum = diag_sum + spiral_bottom_right_corner(i)
	diag_sum = diag_sum + spiral_bottom_left_corner(i)

print("Diag sum =", diag_sum)
