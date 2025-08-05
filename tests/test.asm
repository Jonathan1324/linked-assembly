bits 32

%define t.a nop

section .text
    global _start

_start:
    t.a

a equ 2
b equ a
c equ $
d equ a + c