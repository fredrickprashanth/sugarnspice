input = [1,0,4,5,9,3,2,9]
input = [1,2,-3,4,-2,9,10,2,-5]

max_inc_sum = input[0]
max_inc_start = 0
max_inc_end = 0
start = 0
end = 0
sum = max_inc_sum
for i in range(1, len(input)):
	if sum + input[i] <= input[i]:
		start = i
		end = i
		sum = input[i]
	else:
		sum = sum + input[i]
		end = i
		if sum > max_inc_sum:
			max_inc_end = end
			max_inc_start = start
			max_inc_sum = sum

print "Max sum = ", max_inc_sum
print "Start = ", max_inc_start, " End = ", max_inc_end
print "Elements = ", input[max_inc_start:max_inc_end+1]


	
	
	
	
