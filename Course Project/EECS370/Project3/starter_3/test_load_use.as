        lw      0       1       value1  load reg1 with 15
        add     1       0       2       use reg1 immediately (load-use hazard)
        add     2       2       3       use reg2 (forwarding test)
        nor     3       0       4       use reg3 to compute result
        halt
value1  .fill   15
