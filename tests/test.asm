[BITS 32]

section .text
global _start
_start:
    push 0x00000001
    push 0x00000010
    push 0x00000100
    push 0x00001000
    push 0x00010000
    push 0x00100000
    push 0x01000000
    push 0x10000000