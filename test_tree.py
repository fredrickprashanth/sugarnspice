
class Node:
	def __init__(self, data, left, right):
		self.__data = data
		self.__left = left
		self.__right = right
	def data(self):
		return self.__data
	def left(self):
		return self.__left
	def right(self):
		return self.__right


def Paths(node, path):
	if node is None:
		return
	path.append(node.data())
	Paths(node.left(), path)
	Paths(node.right(), path)
	if node.left() is None and node.right() is None:
		for x in path:
			print(x," ", end="")
		print("")
	path.pop()

#
#      1
#   2     3
# 4    5   6   7
#

n4 = Node(4, None, None)
n5 = Node(5, None, None)
n6 = Node(6, None, None)
n7 = Node(7, None, None)
n3 = Node(3, n6, n7)
n2 = Node(2, n4, n5)
n1 = Node(1, n2, n3)

path=[]
Paths(n1, path)
	
		
