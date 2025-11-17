        lw      0       1       five    load reg1 with 5
        lw      0       2       ten     load reg2 with 10
        beq     1       1       next    branch taken (5 == 5)
        add     0       0       0       should be flushed
next    beq     2       2       end     branch taken (10 == 10)
        add     0       0       0       should be flushed
end     add     1       2       3       reg3 = 5 + 10 = 15
        halt
five    .fill   5
ten     .fill   10
