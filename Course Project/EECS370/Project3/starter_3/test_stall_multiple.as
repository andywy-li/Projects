        lw      0       1       num1    load reg1 with 20
        lw      0       2       num2    load reg2 with 30
        add     1       2       1       reg1 = 20 + 30 = 50 (stall on both)
        lw      0       3       num3    load reg3 with 10
        add     1       3       4       reg4 = 50 + 10 = 60 (stall on reg3)
        halt
num1    .fill   20
num2    .fill   30
num3    .fill   10
