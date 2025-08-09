[bits 32]

%define ENDL 10, 0


section .text
    global _start

_start: \
    int 0x80

section .data
msg db "Hello, World!", ENDL