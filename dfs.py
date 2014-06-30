class Node:
	def __init__(self, data):
		self.__data = data
		self.__neighbors=[]
		self.__metadata = None
	def data(self):
		return self.__data
	def add_neighbor(self, node):
		self.__neighbors.append(node)
	def add_neighbors(self, node_list):
		self.__neighbors.extend(node_list)
	def neighbors(self):
		return self.__neighbors
	def set_metadata(self, metadata):
		self.__metadata = metadata
	def metadata(self):
		return self.__metadata

def PrintPath(path):
	print("Path")
	for node in path:
		print(node.data())

def DFS(node):
	curr_path = []
	curr_path.append(node)
	while len(curr_path) > 0:
		curr_node = curr_path[-1]
		#print(curr_node.data())
		if len(curr_node.neighbors()) > 0:
			all_visited = True
			for neighbor in curr_node.neighbors():
				if not neighbor.metadata():
					#print("Adding to path", neighbor.data())
					curr_path.append(neighbor)
					all_visited = False
					break
			if all_visited:
				curr_path.pop()
				curr_node.set_metadata(True)
		else:
			PrintPath(curr_path)
			curr_path.pop()
			curr_node.set_metadata(True)

n1 = Node(1)
n2 = Node(2)
n3 = Node(3)
n4 = Node(4)
n5 = Node(5)
n6 = Node(6)

n1.add_neighbors([n2, n3])
n2.add_neighbors([n4])
n3.add_neighbors([n5])
n5.add_neighbors([n6])

DFS(n1)

				
		
	
	
	
