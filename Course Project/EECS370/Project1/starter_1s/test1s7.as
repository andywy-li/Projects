	lw	0	1	a	; 0xAAAAAAAA
	lw	0	2	b	; 0x55555555
	nor	1	2	3	; r3=0
	nor	1	1	4	; r4=-1
	nor	2	2	5	; r5=-2
	lw	0	6	val
	lw	0	7	bAddr
	sw	7	6	0	; mem[500]=val
	lw	7	2	0	; r2=val
loop	add	2	1	2	; r2 += a (large positive)
	beq	0	0	loopnd
	beq	0	0	loop	; taken branch negative offset
loopnd	halt
a	.fill	2863311530
b	.fill	1431655765
val	.fill	42
bAddr	.fill	500
