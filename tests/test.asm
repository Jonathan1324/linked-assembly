[bits 32]

%include "inc.asm"

%define ENDL 10, 0


section .text
    global _start

_start: \
    int 0x80
    nop

section .data
msg db "Hello, World!", ENDL