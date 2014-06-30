ones_words = []
for i in range(0, 10):
	ones_words.append("")
ones_words[0] = ""
ones_words[1] = "one"
ones_words[2] = "two"
ones_words[3] = "three"
ones_words[4] = "four"
ones_words[5] = "five"
ones_words[6] = "six"
ones_words[7] = "seven"
ones_words[8] = "eight"
ones_words[9] = "nine"

tens_words = []
for i in range(0, 10):
	tens_words.append("")
tens_words[0] = "ten"
tens_words[1] = "eleven"
tens_words[2] = "twelve"
tens_words[3] = "thirteen"
tens_words[4] = "fourteen"
tens_words[5] = "fifteen"
tens_words[6] = "sixteen"
tens_words[7] = "seventeen"
tens_words[8] = "eighteen"
tens_words[9] = "nineteen"

nty_words=[]
for i in range(0, 10):
	nty_words.append("")
nty_words[1] = "ten"
nty_words[2] = "twenty"
nty_words[3] = "thirty"
nty_words[4] = "forty"
nty_words[5] = "fifty"
nty_words[6] = "sixty"
nty_words[7] = "seventy"
nty_words[8] = "eighty"
nty_words[9] = "ninety"





def convert_to_words(num):
	num_str = ""
	num_i = int(num/1000)
	num_j = num%1000
	if num_i != 0:
		num_str = num_str + ones_words[num_i] + " thousand"
	num = num_j
	num_i = int(num/100)
	num_j = num%100
	if num_i != 0:
		num_str = num_str + ones_words[num_i] + " hundred"
	if num_j != 0:
		if len(num_str) > 0:
			num_str = num_str + " and"
		num = num_j
		num_i = int(num/10)
		num_j = num%10
		if len(num_str) > 0:
			num_str = num_str + " "
		if num_i == 1:
			num_str = num_str + tens_words[num_j]
		else:
			if num_i != 0:
				num_str = num_str + nty_words[num_i] + " " + ones_words[num_j]
			else:
				num_str = num_str + ones_words[num_j]
				

	return num_str

			
		
numbers = range(1, 1001)
count = 0
for num in numbers:
	num_str = convert_to_words(num)
	y = num_str.split(" ")
	for x in y:
		count = count + len(x)
	print(num_str)

print("Total count = ", count)
	
