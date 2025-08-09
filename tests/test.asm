[bits 32]

section .text
    global _start
    global l3

_start:
    db "Hello, World", 0x00
l2:

a equ $ - 1

l3:

section .data
db "DATA"