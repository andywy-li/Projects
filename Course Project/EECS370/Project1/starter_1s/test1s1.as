	lw	0	1	one	; r1=1
	lw	0	2	two	; r2=2
	add	1	2	3	; r3=3
	add	3	2	3	; r3=5 (dest==src)
	lw	0	4	base	; r4 = &data2
	lw	4	5	-1	; r5 = mem[base-1] = data1  (needs sign-extend)
	sw	4	5	1	; mem[base+1] = r5 -> data3
	lw	4	6	1	; r6 = mem[base+1] -> must equal data1
	halt
one	.fill	1
two	.fill	2
data1	.fill	99
data2	.fill	123
data3	.fill	0
base	.fill	data2
