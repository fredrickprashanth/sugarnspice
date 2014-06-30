import sys

input_file = open("max_path_sum3.data", "r")
tree=[]
num_levels = 0
for line in input_file:
	line = line.strip()
	nums_str = line.split(' ')
	for num_str in nums_str:
		tree.append(int(num_str))
	num_levels = num_levels + 1

tree_sum = []
tree_sum.extend(tree)
print(tree_sum)

print("levels = ", num_levels)
num_nodes = 0

for level in range(1, num_levels + 1):
	if level == 1:
		num_nodes = num_nodes + level
		continue
	parent = num_nodes - (level-1)
	two_node_count = 0
	i = num_nodes
	while i < num_nodes + level:
		node_sum = tree[i] + tree_sum[parent]
		#print("node_sum = ", node_sum, "i = ", i, "parent =", parent)
		if node_sum > tree_sum[i]:
			tree_sum[i] = node_sum
		two_node_count = two_node_count + 1
		if two_node_count == 2 and i < (num_nodes + level -1):
			parent = parent + 1
			i = i - 1
			two_node_count = 0
		i = i + 1
	num_nodes = num_nodes + level
		
print(tree_sum)
print("Max is ", max(tree_sum))
