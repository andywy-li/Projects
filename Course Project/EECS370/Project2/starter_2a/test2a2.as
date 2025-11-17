start	add	0	0	0
	lw	0	1	local
	lw	0	2	Global
	sw	0	3	Global
	beq	0	0	done
done	halt
local	.fill	5
data1	.fill	Global
