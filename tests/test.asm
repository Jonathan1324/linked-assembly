[BITS 32]

section .text
global _start
_start:
    mov eax, [data1]

section .data
data1:          dd 0x01234567
data2:          dd 0x89abcdef