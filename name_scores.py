import sys

names_file = open(sys.argv[1], "r")
all_names = []
for line in names_file:
	line = line.strip()
	names = line.split(",")
	all_names.extend(names)

all_names.sort()

total_score = 0
for i in range(0, len(all_names)):
	name = all_names[i]
	score = 0
	for x in range(0, len(name)):
		score = score + ord(name[x]) - ord('A') + 1
	score = score*(i+1)
	total_score = total_score + score
	
print("Total score = ", total_score)
	
