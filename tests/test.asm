[BITS 32]

section .text
global _start
_start:
    push 0x1234
    push eax
    push ax
    push es
    push cs
    push ss
    push ds
    push fs
    push gs
    pop ebx
    pop bx
    pop es
    pop ss
    pop ds
    pop fs
    pop gs