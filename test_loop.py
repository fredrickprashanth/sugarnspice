class Node:
	def __init__(self, data, next):
		self.__data = data
		self.__next = next
	def data(self):
		return self.__data
	def next(self):
		return self.__next
	def set_next(self, next):
		self.__next = next


# Tortoise and Hare algo
# Floyd's algo to detect loops.
def DetectLoop(node):
	slow_node = node
	fast_node = node
	while True:
		if slow_node is None or fast_node is None:
			print "No loops"
			break
		slow_node = slow_node.next()
		fast_node = fast_node.next()
		if fast_node is not None:
			fast_node = fast_node.next()
		if slow_node is not None and slow_node == fast_node:
			print "Looped at ", fast_node.data()
			break


n1 = Node(1, None)
n2 = Node(2, n1)
n3 = Node(3, n2)
n4 = Node(4, n3)
n5 = Node(5, n3)
n6 = Node(6, n5)
n7 = Node(7, n6)

n1.set_next(n3)

DetectLoop(n7)

