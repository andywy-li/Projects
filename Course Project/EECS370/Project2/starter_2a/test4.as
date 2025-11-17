        lw      0       1       BASE
        lw      0       2       V1
        lw      0       3       V2
        lw      0       4       V3
        sw      2       1       0
        sw      3       1       1
        sw      4       1       2
        add     0       0       5
        add     0       0       6
L       lw      0       7       SIZE
        beq     6       7       D
        add     1       6       7
        lw      7       4       0
        add     5       4       5
        lw      0       4       ONE
        add     6       4       6
        beq     0       0       L
D       sw      5       0       SUM
        halt
BASE    .fill   80
V1      .fill   3
V2      .fill   6
V3      .fill   9
SIZE    .fill   3
ONE     .fill   1
SUM     .fill   0

