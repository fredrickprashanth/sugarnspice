
def do_merge(input, left, left_end, right_end, output):
	j = left_end
	i = left
	while i < left_end and j < right_end:
		if input[i] > input[j]:
			output.append(input[j])
			j = j + 1
		else:
			output.append(input[i])
			i = i + 1
	output.extend(input[i:left_end])
	output.extend(input[j:right_end])

def merge_sort(input):
	temp_input = []
	w = 1
	while w < len(input):
		for i in range(0, len(input), 2*w):
			print "w = ", w
			left = i
			left_end = min(i+w, len(input))
			right_end = min(i+2*w, len(input))
			print "do_merge on ", left, left_end, right_end
			do_merge(input, left, left_end, right_end, temp_input) 
			print "After merge input =", input
			print "After merge temp_input =", temp_input
		del input[:]
		input.extend(temp_input)
		del temp_input[:]
		w = w*2
	return input

input = [9, 0, 1, 8, -1, 2, -2, 29, -90]
print "Sorted input = ", merge_sort(input)
				
			
		
		
