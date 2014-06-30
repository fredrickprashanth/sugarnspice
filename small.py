import sys
import re
import math
input_file=open(sys.argv[1],"r")
pts={}
for line in input_file:
	fields = re.split("[\ ]*", line)
	pts[int(fields[0])]=(float(fields[1]),float(fields[2]))

dists={}

def d(pt1, pt2):
	return math.sqrt((pt1[0]-pt2[0])**2 + (pt1[1]-pt2[1])**2)
for pt1 in pts:
	for pt2 in pts:
		if pt1<pt2:
			dists[(pt1,pt2)]=d(pts[pt1],pts[pt2])
print dists
for pt1 in pts:
	d_l = []
	for pt2 in pts:
		if pt1 == pt2:continue
		d_t = (min(pt1,pt2),max(pt1,pt2))
		d_l.append((dists[d_t],pt2))
	d_l = sorted(d_l, key=lambda (x,y):x)
	p_str = [str(pt1)]
	for pt_d in d_l:
		p_str.append(str(pt_d[1]))
	print " ".join(p_str)


