[BITS 32]

section .text
global _start
_start:
    mov eax, 1
    ret

section .data
bss1:          reso 1
bss2:          resy 1
bss3:          resz 1