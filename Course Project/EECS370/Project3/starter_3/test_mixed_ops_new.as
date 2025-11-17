        lw      0       1       val1    load reg1 with 12
        lw      0       2       val2    load reg2 with 8
        add     1       2       3       reg3 = 12 + 8 = 20
        nor     1       2       4       reg4 = nor(12, 8)
        add     3       4       5       reg5 = 20 + reg4
        beq     0       5       skip    check if reg5 == 0 (not taken)
        nor     5       5       6       reg6 = nor(reg5, reg5)
skip    halt
val1    .fill   12
val2    .fill   8
