	lw	0	2	entry
	jalr	2	3
	halt
entry	add	0	0	4
	lw	0	5	tramp
	jalr	5	6
	jalr	3	7
	halt
tramp	add	0	0	1
	jalr	6	7
