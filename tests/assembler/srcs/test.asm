[bits 32]

%define ENDL 10, 0

section .text
    global _start

_start:
    int msg
    dw msg
    dd msg
    dq msg
    
section .data

msg db "Hello, world!", ENDL
msg_len equ $ - msg