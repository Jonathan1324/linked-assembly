[bits 32]

%define ENDL 10, 0

section .text
    global _start

_start:
    ;mov eax, 1
    ;mov ebx, 0
    int 0

    ;lea eax, err

section .data

d db 0xff

msg db "Hello, World!", ENDL

err db "Error found!", ENDL