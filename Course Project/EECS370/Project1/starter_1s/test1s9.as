	lw	0	2	gateA
	jalr	2	4
	halt
gateA	add	0	0	6
	lw	0	3	gateB
	jalr	3	5
	jalr	4	7
	halt
gateB	add	0	0	2
	jalr	5	7
