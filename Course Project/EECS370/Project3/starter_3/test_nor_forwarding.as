        lw      0       1       mask    load reg1 with 255
        nor     1       0       2       reg2 = nor(255, 0) = -256
        nor     2       0       3       reg3 = nor(-256, 0) = 255 (forward from prev)
        add     3       0       4       reg4 = 255 + 0 = 255 (forward from prev)
        halt
mask    .fill   255
