[bits 32]

%define ENDL 10, 0

section .text
    global _start

_start:
    ;mov eax, 1
    ;mov ebx, 0
    int msg_len

    ;lea eax, err

section .data

msg_start equ $
msg db "Hello, World!", ENDL
msg_len equ $ - msg_start

err db "Error found!", ENDL
