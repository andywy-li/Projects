        lw      0       1       neg10   load reg1 with -10
        lw      0       2       pos5    load reg2 with 5
        add     1       2       3       reg3 = -10 + 5 = -5
        nor     3       0       4       reg4 = nor(-5, 0) = 4
        add     3       4       5       reg5 = -5 + 4 = -1
        halt
neg10   .fill   -10
pos5    .fill   5
