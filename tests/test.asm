bits 32

section .text
    global _start

_start:
    mov eax, 9 + 0b1010

a equ 0x1234
b equ $ + 1