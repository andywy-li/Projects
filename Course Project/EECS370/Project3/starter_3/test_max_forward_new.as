        lw      0       1       val     load reg1 with 25
        add     1       0       2       reg2 = 25 (EX to EX forward)
        add     2       0       3       reg3 = 25 (MEM to EX forward)
        add     3       0       4       reg4 = 25 (WB to EX forward)
        add     4       0       5       reg5 = 25 (no forward needed)
        halt
val     .fill   25
