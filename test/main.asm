bits 16

section .text
extern testExtern
global rone
rone:
    mov eax, 1
    ret

section .data
t:      db      0x0
a:      dw      0x1
r:      dq      0x53a
res:    rest    0
        resb    5
test:   dd      9

section .text
; test function
global test
test:
    bits 32
    mov eax, 0      ; test comment
    call rone
    mov [res], eax