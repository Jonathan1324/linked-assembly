section .text
    extern testExtern
    extern testExtern2
    extern testExtern3

label:          resb 0x10
test:           db 1
tesw:           dw 0
t:              db 11
lol:            resw 1
lolol:          db 0
lolol2:         db 0

section .data

d:              rest 10
d2:             rest 10
d3:             rest 10
longverylongdataNamejustForT3st:    dw 1
anotherlongName:                    dw 2