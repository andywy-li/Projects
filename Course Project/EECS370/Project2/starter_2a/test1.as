        lw      0       1       A
        lw      0       2       B
        add     1       2       3
        lw      0       4       EXTA
        add     3       4       3
        beq     3       0       skip
        add     3       3       3
skip    sw      3       0       RES
        halt
A       .fill   11
B       .fill   7
RES     .fill   0

