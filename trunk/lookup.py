dat  =['NULL', 'LE', 'LT', 'LI', 'LA', 'LN', 'LM', 'LS', 'LU', 'LR', 'LW', 'LD', 'LK', 'LG', 'LO', 'LH', 'LV', 'LF', 'SPACE', 'LL', 'ENTER', 'LP', 'LJ', 'LB', 'LX', 'NULL', 'LY', 'LZ', 'LQ', 'CTRL', 'BSCAPE', 'N5', 'N4', 'TOGGLEGRP', 'N3', 'NULL', 'NULL', 'LCURLY', 'N2', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'N1', 'N6', 'NULL', 'FSLASH', 'NULL', 'NULL', 'SEMICOLON', 'NULL', 'NULL', 'N7', 'RCURLY', 'NULL', 'ALT', 'N8', 'NULL', 'N9', 'N0', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'LPAREN', 'NULL', 'NULL', 'NULL', 'NULL', 'QUESTION', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'PERIOD', 'NULL', 'NULL', 'RSQUARE', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'APOS', 'NULL', 'NULL', 'DASH', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'LSQUARE', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'SHFT', 'NULL', 'COMMA', 'NULL', 'NULL', 'NULL', 'NULL', 'COLON', 'NULL', 'NULL', 'NULL', 'BSLASH', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'F12', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'F11', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'F5', 'F4', 'NULL', 'F3', 'NULL', 'NULL', 'NULL', 'F2', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'F1', 'F6', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'F7', 'NULL', 'NULL', 'NULL', 'F8', 'NULL', 'F9', 'F10']


def decode(c):
    ptr = 0
    while c:
        a = c[0]
        c = c[1:]
        if a == '.':
            ptr = ptr *2 + 1
        if a == '-':
            ptr = ptr * 2 + 2
    return dat[ptr]
