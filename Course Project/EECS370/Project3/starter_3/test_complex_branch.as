        lw      0       1       val1    load reg1 with 8
        lw      0       2       val2    load reg2 with 4
        add     1       2       3       reg3 = 8 + 4 = 12
        add     2       2       4       reg4 = 4 + 4 = 8
        beq     3       4       wrong   branch not taken (12 != 8)
        nor     3       4       5       reg5 = nor(12, 8)
        beq     1       4       right   branch taken (8 == 8)
        add     0       0       0       should be flushed
wrong   halt                            should not reach
right   halt
val1    .fill   8
val2    .fill   4
