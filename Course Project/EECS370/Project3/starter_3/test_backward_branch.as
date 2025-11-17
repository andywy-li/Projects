        lw      0       1       neg2    load reg1 with -2
        lw      0       2       one     load reg2 with 1
loop    add     1       2       1       increment reg1 by 1
        beq     0       1       done    branch when reg1 == 0
        beq     0       0       loop    always branch back to loop
done    halt
neg2    .fill   -2
one     .fill   1
