        lw      0       1       value   load reg1 with 42
        noop                            no operation
        add     1       0       2       use reg1 after noop (test noop handling)
        noop
        noop
        add     2       2       3       reg3 = 42 + 42 = 84
        halt
value   .fill   42
