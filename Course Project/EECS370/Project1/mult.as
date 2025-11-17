	lw	0	1	mcand	; r1 = mcand
	lw	0	2	mplier	; r2 = mplier
	add	0	0	3	; r3 = 0 (result)
	lw	0	4	one	; r4 = mask = 1
	lw	0	5	limit	; r5 = 1<<16 = 65536
loop	nor	2	2	6	; r6 = ~r2
	nor	4	4	7	; r7 = ~r4
	nor	6	7	6	; r6 = r2 & r4   (since ~(~r2 | ~r4))
	beq	6	0	noAdd	; if (r2 & r4) == 0 -> skip
	add	3	1	3	; result += mcand
noAdd	add	1	1	1	; mcand <<= 1
	add	4	4	4	; mask   <<= 1
	beq	4	5	done	; if mask == 1<<16 -> finish 16 rounds
	beq	0	0	loop	; continue
done	halt
one	.fill	1
limit	.fill	65536			; 1<<16 (do 16 bit-positions: 0..15)
mcand	.fill	6203
mplier	.fill	1429
