	lw	0	1	big
	lw	0	2	small
	add	1	2	3
	lw	0	4	addr
	sw	4	3	-1	; store with negative offset
	lw	4	5	-1	; reload -> should equal r3
	halt
big	.fill	32767
small	.fill	-32768
addr	.fill	50
