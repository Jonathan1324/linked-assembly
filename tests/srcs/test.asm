[bits 32]

%define ENDL 10, 0

section .text
    global _start

_start:
    int msg
    
section .data

msg db "Hello, world!", ENDL
msg_len equ $ - msg