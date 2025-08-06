bits 32

section .text
    global _start

_start:
    nop
    db a

a equ 2
b equ a
c equ $
d equ b + c

section .data

msg db "Hello, World!", 0