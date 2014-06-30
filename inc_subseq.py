input = [4, -3, -1, 0, 9, -10, -1]

inc_start = 0
inc_end = 0
inc_len = 1
sum = input[0]
start = 0
end = 0

prev_i = input[0]

for i in range(1,len(input)):
	if prev_i > input[i]:
		start = i
		end = i
	else:
		end = i
		if (end - start) > (inc_end - inc_start):
			inc_start = start
			inc_end = end
	prev_i = input[i]				

print "Seq = ", input[inc_start:inc_end+1]


		
	
	
	
