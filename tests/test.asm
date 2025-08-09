[bits 32]

section .text
    global _start

_start:
    db "Hello, World", 0x00
l2:

section .data
db "DATA"