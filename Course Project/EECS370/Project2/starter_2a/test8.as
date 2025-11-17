        lw      0       1       BASE
        lw      0       2       EXP
        add     0       0       3
        lw      0       4       ONE
        add     3       4       3
        add     0       0       5
P      beq     5       2       D
        add     0       0       6
        add     0       0       7
M      beq     7       1       ME
        add     6       3       6
        add     7       4       7
        beq     0       0       M
ME     add     6       0       3
        add     5       4       5
        beq     0       0       P
D      sw      3       0       RES
        halt
BASE    .fill   2
EXP     .fill   3
ONE     .fill   1
RES     .fill   0

