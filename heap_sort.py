
def swap_input(input, x, y):
	t = input[x]
	input[x] = input[y]
	input[y] = t

def sift_down(input, i, input_len):
	while True:
		right = 2*i + 2
		left = right - 1
		print "i=", i, "left=", left, "right=", right
		largest = i
		if left > 0 and left < input_len and input[left] > input[largest]:
			largest = left
		if right > 0 and right < input_len and input[right] > input[largest]:
			largest = right
		if largest != i:
			swap_input(input, i, largest)
			i = largest
		else:
			break
		

def heap_sort(input):
	for i in range(len(input)/2, -1, -1):
		sift_down(input, i, len(input))

	input_len = len(input)
	for i in range(0, len(input)-1):
		swap_input(input, 0, input_len - 1)
		sift_down(input, 0, input_len - 1)
		input_len = input_len - 1

input = [9, 4, 1, 4, 3, 5, 0, 20, -2]
		
heap_sort(input)

print "Input = ", input 

			
