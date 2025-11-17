	lw	0	1	val	; r1 = 99
	lw	0	2	dst	; r2 = &dstSlot
	sw	0	1	dst	; mem[dstSlot] = 99
	lw	0	3	dst	; r3 = mem[dstSlot] -> 99
	halt
val	.fill	99
dst	.fill	dstS
dstS	.fill	0
