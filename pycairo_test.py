import cairo
import math
import random

WIDTH, HEIGHT = 256, 256
surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, WIDTH, HEIGHT)
ctx = cairo.Context(surface)

ctx.scale(WIDTH, HEIGHT)
DIV = 100
ctx.set_source_rgb(1, 1, 1)
ctx.rectangle(0, 0, 1, 1)
ctx.fill()


ctx.set_line_width(0.1)

for i in range(0,DIV):
	ret = random.random()
	print ret
	if ret > 0.5:
		continue
	ctx.set_source_rgb (random.random(),
			random.random(),
			random.random())
	x = float(i)/DIV
	ctx.move_to(x, 0.25)
	ctx.line_to(x, 0.5)
	ctx.stroke()
surface.write_to_png("mytry.png")
