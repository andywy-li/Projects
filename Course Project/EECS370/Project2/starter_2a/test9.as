        lw      0       1       ARR
        lw      0       2       SIZE
        lw      0       3       V1
        lw      0       4       V2
        lw      0       5       V3
        sw      3       1       0
        sw      4       1       1
        sw      5       1       2
        add     0       0       6
        add     0       0       7
S      beq     7       2       E
        add     1       7       3
        lw      3       4       0
        add     6       4       6
        lw      0       4       ONE
        add     7       4       7
        beq     0       0       S
E      sw      6       0       SUM
        halt
ARR     .fill   200
SIZE    .fill   3
V1      .fill   10
V2      .fill   20
V3      .fill   30
ONE     .fill   1
SUM     .fill   0

