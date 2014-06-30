class Node:
	def __init__(self, left, right, data):
		self.left = left
		self.right = right
		self.data = data
	def data(self):
		return data

n1 = Node(None, None, 1)
n2 = Node(None, None, 2)
n3 = Node(n1, n2, 3)

def inorder(n):
	if n.left != None:
		inorder(n.left)
	print "inorder data = ", n.data
	if n.right != None:
		inorder(n.right)

def preorder(n):
	print "preorder data = ", n.data
	if n.left != None:
		preorder(n.left)
	if n.right != None:
		preorder(n.right)
def postorder(n):
	if n.left != None:
		postorder(n.left)
	if n.right != None:
		postorder(n.right)
	print "postorder data = ", n.data

inorder(n3)
preorder(n3)
postorder(n3)

