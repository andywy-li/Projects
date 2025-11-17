        lw      0       1       five    load reg1 with 5
        lw      0       2       addr    load reg2 with address
        sw      2       1       0       store reg1 value to memory[reg2]
        lw      2       3       0       load from memory[reg2] into reg3
        add     3       3       4       use loaded value (should be 5+5=10)
        halt
five    .fill   5
addr    .fill   data
data    .fill   0
