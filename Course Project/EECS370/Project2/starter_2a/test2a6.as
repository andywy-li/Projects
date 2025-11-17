	add	4	6	7
	lw	4	5	7
	add	0	0	0
	nor	3	1	2
	sw	1	0	apple
	sw	5	1	12342
	lw	5	7	-3
apple	beq	2	2	baby
	beq	0	0	-5
	beq	1	6	baby
baby	lw	1	7	cat
cat	jalr	3	5
	jalr	4	4
	add	5	2	7
dog	noop
	halt
eat	.fill	-888
	.fill	10000
