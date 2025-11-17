	lw	0	1	addr
	lw	0	2	one
	lw	0	3	two
	sw	1	2	0	; mem[addr]=1
	lw	1	4	0	; r4=1
	sw	1	3	0	; mem[addr]=2
	lw	1	5	0	; r5=2
	add	4	5	6	; r6=3
	halt
addr	.fill	40
one	.fill	1
two	.fill	2
