        lw      0       1       seven   load reg1 with 7
        add     1       1       2       reg2 = 7 + 7 = 14 (hazard on reg1)
        add     2       2       3       reg3 = 14 + 14 = 28 (hazard on reg2)
        add     3       3       4       reg4 = 28 + 28 = 56 (hazard on reg3)
        halt
seven   .fill   7
