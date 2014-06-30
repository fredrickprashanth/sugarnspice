struct x {
	long a;
	unsigned int b1;
	unsigned int b2:1;
};


struct x y;
int main(void) {
	struct x _x;
	_x.b2 = 1;
	y.b2 = 1;
	return 0;
}
