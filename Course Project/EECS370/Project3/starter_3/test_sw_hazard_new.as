        lw      0       1       val     load reg1 with 99
        add     1       1       2       reg2 = 99 + 99 = 198
        lw      0       3       addr    load reg3 with address
        sw      3       2       0       store reg2 to memory (needs forwarding)
        lw      3       4       0       load back into reg4
        halt
val     .fill   99
addr    .fill   data
data    .fill   0
