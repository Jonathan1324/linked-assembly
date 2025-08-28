[bits 32]

%define ENDL 10, 0

section .text
    global _start

_start:
    mov eax, 1
    mov ebx, 0
    int 0x80
    
section .data

msg db "Hello, world!", ENDL
msg_len equ $ - msg