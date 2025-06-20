[BITS 32]

section .text
global _start
_start:
    int 0b1010
    int 0d10
    int 0h10
    int 0o10
    int 0q10
    int 0t10
    int 0x10
    int 0y10
    int 10