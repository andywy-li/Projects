        lw      0       1       ten     load reg1 with 10
        lw      0       2       five    load reg2 with 5
        add     1       2       3       reg3 = 10 + 5 = 15
        beq     3       3       target  branch taken (reg3 == reg3, needs forwarding)
        add     1       1       1       should be flushed
target  halt
ten     .fill   10
five    .fill   5
