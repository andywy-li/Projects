        lw      0       1       RET
        lw      0       2       ARG
        jalr    3       1
        sw      2       0       OUT
        halt
FUN     add     2       2       2
        sw      2       0       TMP
        jalr    1       3
RET     .fill   FUN
ARG     .fill   5
OUT     .fill   0
TMP     .fill   0

