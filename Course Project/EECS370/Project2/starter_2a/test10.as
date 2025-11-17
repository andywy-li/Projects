        lw      0       1       maxP
        lw      0       2       maxN
        add     1       2       3
        sw      3       0       SUM
        lw      0       4       ONE
        add     1       4       5
        sw      5       0       OVF
        lw      0       6       NEG1
        add     2       6       7
        sw      7       0       UNF
        halt
maxP    .fill   32767
maxN    .fill   -32768
ONE     .fill   1
NEG1    .fill   -1
SUM     .fill   0
OVF     .fill   0
UNF     .fill   0

