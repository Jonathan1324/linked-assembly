section .text
    global _start

_start:
    db 0, 1
    dw 2, 3
    dd 4, 5
    dq 6, 7

section .data

msg     db 'h', 'e', 'l', 'l', 'o', '!', 0
msg2:   db 't', 'e', 's', 't', 0
