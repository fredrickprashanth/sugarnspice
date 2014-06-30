
class Node:
	def __init__(self, data):
		self.__data = data
		self.__neighbors = []
	def data(self):
		return self.__data
	def add_neighbor(self, x):
		self.__neighbors.append(x)
	def neighbors(self):
		return self.__neighbors


my_nodes = []
for i in range(0,10):
	n = Node(i)
	my_nodes.append(n)

my_nodes[0].add_neighbor(my_nodes[1])
my_nodes[0].add_neighbor(my_nodes[2])
my_nodes[0].add_neighbor(my_nodes[3])

my_nodes[1].add_neighbor(my_nodes[4])
my_nodes[1].add_neighbor(my_nodes[5])
my_nodes[1].add_neighbor(my_nodes[6])

my_nodes[4].add_neighbor(my_nodes[6])
my_nodes[6].add_neighbor(my_nodes[7])
my_nodes[6].add_neighbor(my_nodes[3])

def BFS(node):
	visited_nodes = {}
	to_visit = []

	to_visit.append(node)
	visited_nodes[node] = 1
	while len(to_visit) > 0:
		# For DFS, pop like a stack.
		n = to_visit.pop()
		# For BFS, pop like a queue.
		#n = to_visit.pop(0)
		print "Node = ", n.data()
		for m in n.neighbors():
			if m not in visited_nodes:
				to_visit.append(m)
				visited_nodes[m] = 1
			else:
				print "Loop at ", n.data()
				# Using DFS, you can detect
				# loops.

	
BFS(my_nodes[0])
	
	
