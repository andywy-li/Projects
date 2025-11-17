        lw      0       1       VAL1
        lw      0       2       VAL2
        lw      0       3       MASK
        nor     1       2       4
        nor     4       3       4
        sw      4       0       OUT
        halt
VAL1    .fill   13
VAL2    .fill   6
MASK    .fill   12
OUT     .fill   0

