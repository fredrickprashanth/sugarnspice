class Node:
	def __init__(self, data, neighbors):
		self.__data = data
		self.__neighbors = neighbors
	
	def data(self):
		return self.__data
	def neighbors(self):
		return self.__neighbors
	def add_neighbor(self, neighbor):
		self.__neighbors.append(neighbor)


def Print(node, bfs=False):
	visited_nodes = {}
	to_visit = []
	to_visit.append(node)
	while len(to_visit) > 0:
		if bfs:
			curr_node = to_visit.pop(0)
		else:
			curr_node = to_visit.pop()
		if curr_node in visited_nodes:
			continue
		print(curr_node.data())
		for neighbor in curr_node.neighbors():
				to_visit.append(neighbor)
		visited_nodes[curr_node] = True

	
n1 = Node(1, [])
n2 = Node(2, [n1])
n3 = Node(3, [n2, n1])
n4 = Node(4, [n3])
n5 = Node(5, [n2, n3])
n1.add_neighbor(n4)
n6 = Node(6, [])
n1.add_neighbor(n6)
n7 = Node(7, [])
n6.add_neighbor(n7)

print("DFS")
Print(n5)
print("BFS")
Print(n5, True)
	
		
