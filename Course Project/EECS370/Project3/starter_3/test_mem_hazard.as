        lw      0       1       addr    load reg1 with address
        lw      1       2       0       load from memory[reg1+0]
        add     2       2       3       use loaded value (mem-to-use hazard)
        sw      1       3       0       store result back
        halt
addr    .fill   data
data    .fill   100
