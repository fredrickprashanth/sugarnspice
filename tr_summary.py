#!/usr/bin/python
import sys
import re
from types import *
from datetime import datetime,date,time,timedelta
import cairo

if len(sys.argv) < 3:
	print "tr_summary.py <file> <cpu>"
	sys.exit(1)

file_input = open(sys.argv[1], "r")
cpu = int(sys.argv[2])

ps_list = {}
ps_name = ""
prev_ps_name = ""
ts = None
prev_ts = None
first_ts = None
num_evts = 0
event_re = re.compile(r'Event:\s+(.+)')
info_re = re.compile(r'Info:\s+(\w+)')
cpu_re  = re.compile(r'Cpu:\s+(\d)')
ts_re = re.compile(r'\d*\s*(\w{3})\s+(\d+) (\d{2}):(\d{2}):(\d{2}).(\d+)')
ioctl_re = re.compile(r'Ioctl\s+(.+)')
info_re = re.compile(r'\S+\s+\S+\s+(\S+)')
line_count = 0
first_line = ""

def parse_event_info(event_str):
	info_match = info_re.search(event_str)
	if info_match is None:
		return None
	info_grps = info_match.groups()
	return info_grps[0]
	
def parse_ioctl(line):
	ioctl_match = ioctl_re.search(line)
	if ioctl_match is None:
		return None
	ioctl_grps = ioctl_match.groups()
	return ioctl_grps[0]

def parse_ts(line):
	ts_match = ts_re.search(line) 
	if ts_match is None:
		return None
	ts_grps = ts_match.groups()
	ts_temp = str(date.today().year)+ " "+ \
			ts_grps[0] + " " + ts_grps[1] + " " + \
			ts_grps[2] + ":" + ts_grps[3] + ":" + ts_grps[4] + \
			"." + "%06d"%(int(ts_grps[5])*1000)
	ts = datetime.strptime(ts_temp, "%Y %b %d %H:%M:%S.%f")
	return ts
	
def parse_event(line):
	event_match_re = event_re.search(line)
	if event_match_re is None:
		return None
	event_grps = event_match_re.groups()
	event_str = event_grps[0].strip()
	return event_str

def parse_cpu(line):
	cpu_re_match = cpu_re.search(line)
	if cpu_re_match is None:
		return None
	this_cpu = int(cpu_re_match.groups()[0])
	return this_cpu

irq_list = []
syscall_list = []
ioctl_list = []

def print_progress_dots():
	print_progress_dots.line_count += 1
	lc = print_progress_dots.line_count
	if lc & (4096*16-1) == 0:
		sys.stderr.write(".")
		sys.stderr.flush()
print_progress_dots.line_count = 0

class Event:
	def __init__(self, ts, event_str, event_info):
		self.ts = ts
		self.event_str = event_str
		self.event_info = event_info

event_list = []
last_line = None
for line in file_input:
	line = line.strip()
	print_progress_dots()
	this_cpu = parse_cpu(line)
	if this_cpu is None or this_cpu != cpu:
		continue

	if line.find("Ioctl") >= 0:
		ioctl_cmd = parse_ioctl(line)
		ioctl_list.append(ioctl_cmd)
		last_line = line
		continue
	elif line.find("Event") >= 0:
		event_str = parse_event(line)
		last_line = line
	else:
		continue

	ts = parse_ts(line)
	info_str = parse_event_info(event_str)
	if event_str.find("Syscall_entry") >= 0:
		syscall_list.append(info_str)
		event_list.append(Event(ts, "Syscall", info_str))
	elif event_str.find("Irq_entry") >= 0 and event_str.find("SoftIrq_entry") == -1:
		irq_list.append(info_str)
		event_list.append(Event(ts, "Irq", info_str))
	elif event_str.find("Sched_change") >= 0:
		event_str = "Sched_change"
		prev_ps_name = ps_name
		ps_name = info_str
		if len(prev_ps_name) > 0:
			prev_ts = ts
			ts = parse_ts(line)
			if prev_ts is None:
				prev_ts = ts
			if prev_ps_name not in ps_list:
				ps_list[prev_ps_name] = []
			ps_list[prev_ps_name].append((prev_ts, ts, syscall_list, irq_list, ioctl_list))
		irq_list = []
		syscall_list = []	
		ioctl_list = []
		event_list.append(Event(ts, "Sched_change", info_str))

def print_counts(list, isint):
	count_dict = {}
	for x in list:
		if isint:
			x = int(x)
		if not x in count_dict:
			count_dict[x] = 0
		count_dict[x] = count_dict[x] + 1
	for x in count_dict.keys():
		print x, count_dict[x]
	
ps_sorted = sorted(ps_list.keys(), key=lambda x:len(ps_list[x]), reverse=True)

syscall_tbl = {}
irq_tbl = {}
ictl_tbl = {}
glbl_irq_tbl = {}
print "Name, Num sched, Total time(secs)"
for ps_name in ps_sorted:
	ps_td = map(lambda x: x[1] - x[0], ps_list[ps_name])
	total_td = reduce(lambda x,y:x+y, ps_td)
	print ps_name, len(ps_list[ps_name]), total_td.total_seconds()

	#syscalls_list = reduce(lambda x,y: x+y, map(lambda x: x[2], ps_list[ps_name]))
	syscall_tbl.clear()
	irq_tbl.clear()
	ictl_tbl.clear()
	prev_ctx_tpl = None

	for ctx_tpl in ps_list[ps_name]:
		for sr in ctx_tpl[2]:
			sr = int(sr)
			if not sr in syscall_tbl:
				syscall_tbl[sr] = 0
			syscall_tbl[sr] = syscall_tbl[sr] + 1
			
		for irqn in ctx_tpl[3]:
			irqn = int(irqn)
			if not irqn in irq_tbl:
				irq_tbl[irqn] = 0
			irq_tbl[irqn] = irq_tbl[irqn] + 1
			if irqn not in glbl_irq_tbl:
				glbl_irq_tbl[irqn] = 0
			glbl_irq_tbl[irqn] = glbl_irq_tbl[irqn] + 1
		for ict_l in ctx_tpl[4]:
				if not ict_l in ictl_tbl:
					ictl_tbl[ict_l] = 0
				ictl_tbl[ict_l] = ictl_tbl[ict_l] + 1	
		if prev_ctx_tpl is not None:
			prev_ctx_t_end = prev_ctx_tpl[1]
			curr_ctx_t_start = ctx_tpl[0]
			if len(prev_ctx_tpl[2])>0:
				last_sr = int(prev_ctx_tpl[2][len(prev_ctx_tpl[2])-1])
				t_latency = curr_ctx_t_start - prev_ctx_t_end
				if t_latency.total_seconds() > 0.1:
						print "lat", prev_ctx_t_end.strftime("%H:%M:%S.%f"), \
							curr_ctx_t_start.strftime("%H:%M:%S.%f"), \
							"sys_%d"%last_sr, t_latency.total_seconds() 

		t_run = ctx_tpl[1] - ctx_tpl[0]
		if t_run.total_seconds() > 0.01: # 1 jiffy
			print "ctx", ctx_tpl[0].strftime("%H:%M:%S.%f"), \
					ctx_tpl[1].strftime("%H:%M:%S.%f"), \
					t_run.total_seconds()
		prev_ctx_tpl = ctx_tpl 
		
		print_progress_dots()
	
	print "Syscalls count"
	for sr in syscall_tbl:
		print sr, syscall_tbl[sr]
	print "Ioctls count"
	for ictl in ictl_tbl:
		print ictl, ictl_tbl[ictl]
	print "Irqs count"
	for irqn in irq_tbl:
		print irqn, irq_tbl[irqn]

	print "======================================="


file_input.seek(0)
first_ts = None
while first_ts is None:
	first_line = file_input.readline()
	if parse_event(first_line) is not None:
		first_ts = parse_ts(first_line)
if last_line is None:
	print "No data"
	sys.exit(0) 
last_ts = parse_ts(last_line)
total_trace_time = last_ts - first_ts

ps_ts_list = []
print ""
print "Irq stats"
print "======================================="
for irqn in glbl_irq_tbl:
	print irqn, glbl_irq_tbl[irqn]
print ""
print "Top timetakers"
print "Name, Num sched, Total time(secs), Total time(%)"
print "======================================="
for ps_name in ps_sorted:
	ps_evts = map(lambda x: x[2], ps_list[ps_name])
	ps_td = map(lambda x: x[1] - x[0] + timedelta(milliseconds=len(x[2])*0.001), ps_list[ps_name])
	total_td = reduce(lambda x,y:x+y, ps_td)
	if total_td.total_seconds() > 0:
		print ps_name, len(ps_list[ps_name]), total_td.total_seconds(), total_td.total_seconds()*100/total_trace_time.total_seconds()
		ps_ts_list.append((ps_name, total_td.total_seconds()))

ps_sorted_ts = sorted(ps_ts_list, key=lambda x:x[1], reverse = True)
print ""
print "Name, Time(perc)"
print "======================================="
for (ps_name,ts) in ps_sorted_ts:
	perc = ts*100/total_trace_time.total_seconds()
	if perc > 0.1:
		print ps_name, perc


print ""
print "======================================="
print "Total traced time", total_trace_time.total_seconds()

sys.stderr.write("\n")





# Cairo graphics
width = total_trace_time.total_seconds()*100
surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, 256)
ctx = cairo.Context(surface)

ctx.scale(1, 1)
ctx.set_source_rgb(1, 1, 1)

ctx.rectangle(0, 0, 1, 1)
ctx.fill()


ps_color = {}
irq_color = {}

ctx_x = 0
def set_ctx_ps_color(ps_name):
	if ps_name not in ps_color:
		ps_color[ps_name] = (random.random(),
					random.random(),
					random.random())
	ctx.set_source_rgb(ps_color[ps_name][0],
				ps_color[ps_name][1],
				ps_color[ps_name][2])
def set_ctx_irq_color(irq_no):
	if irq_no not in irq_color:
		irq_color[irq_no] = (random.random(),
					random.random(),
					random.random())
	ctx.set_source_rgb(irq_color[irq_no][0],
				irq_color[irq_no][1],
				irq_color[irq_no][2])
curr_ps = None
for evt in event_list:
	if evt.event_str == "Sched_change":
		ps_name = evt.event_info
		curr_ps = ps_name
		set_ctx_ps_color(ps_name)
		ctx.move_to(ctx_x, 0.15)
		ctx.line_to(ctx_x, 0.95)
		ctx_x = ctx_x + 1
	elif evt.event_str == "Syscall":
		if curr_ps == None:
			continue
		set_ctx_ps_color(curr_ps)
		ctx.move_to(ctx_x, 0.25)
		ctx.line_to(ctx_x, 0.75)
		ctx_x = ctx_x + 1
	elif evt.event_str == "Irq":
		set_ctx_irq_color(evt.event_info)
		ctx.move_to(ctx_x, 0.20)
		ctx.line_to(ctx_x, 0.80)
		ctx_x = ctx_x + 1
	ctx.stroke()

surface.write_to_png("trace.png")



