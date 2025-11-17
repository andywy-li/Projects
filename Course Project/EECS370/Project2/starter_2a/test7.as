        lw      0       1       V1
        lw      0       2       V2
        lw      0       3       V3
        beq     1       0       Z1
        beq     2       0       Z2
        beq     3       0       Z3
        add     1       2       4
        add     4       3       4
        beq     0       0       DN
Z1      add     2       3       4
        beq     0       0       DN
Z2      add     1       3       4
        beq     0       0       DN
Z3      add     1       2       4
DN      sw      4       0       RES
        halt
V1      .fill   -5
V2      .fill   0
V3      .fill   3
RES     .fill   0

