        lw      0       1       CNT
        add     0       0       3
L       beq     1       0       D
        lw      0       2       ONE
        add     3       1       3
        add     1       2       1
        beq     0       0       L
D       sw      3       0       RES
        halt
CNT     .fill   4
ONE     .fill   -1
RES     .fill   0

