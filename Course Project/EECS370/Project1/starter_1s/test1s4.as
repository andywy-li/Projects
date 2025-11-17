	lw	0	1	zero
	lw	0	2	one
	lw	0	3	neg1
	nor	1	1	4	; NOR(0,0) = ~0 = -1
	nor	1	2	5	; NOR(0,1) = 0
	nor	2	2	6	; NOR(1,1) = ~1 = -2
	nor	2	3	7	; NOR(1,-1) = 0
	halt
zero	.fill	0
one	.fill	1
neg1	.fill	-1
