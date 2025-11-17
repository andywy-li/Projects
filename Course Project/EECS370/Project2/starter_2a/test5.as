        lw      0       1       X
        lw      0       2       Y
        lw      0       3       Z
        beq     1       2       E12
        beq     1       3       E13
        add     1       2       4
        add     4       3       4
        beq     0       0       F
E12     add     1       1       4
        add     4       3       4
        beq     0       0       F
E13     add     1       1       4
        add     4       2       4
F       sw      4       0       RES
        halt
X       .fill   7
Y       .fill   7
Z       .fill   3
RES     .fill   0

