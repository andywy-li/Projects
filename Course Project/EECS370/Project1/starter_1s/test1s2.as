	lw	0	1	one
	lw	0	2	two
	add	1	2	3	; r3=3
	beq	3	2	skip2	; taken
	add	2	2	4	; should be skipped
	add	2	2	4	; should be skipped
skip2	beq	0	0	far
	add	1	1	1	; skipped
far	beq	0	0	back
	halt
back	beq	0	0	end
	halt
end	halt
one	.fill	1
two	.fill	2
