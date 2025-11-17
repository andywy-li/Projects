        lw      0       1       base    load reg1 with base address
        lw      1       2       0       load from memory[base+0]
        add     2       0       3       reg3 = value + 0
        sw      1       3       0       store reg3 back to memory[base+0]
        halt
base    .fill   data
data    .fill   100
