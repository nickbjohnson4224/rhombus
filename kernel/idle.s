[bits 32]

global idle
idle:
	sti
	hlt

global halt:
halt:
	cli
	hlt
