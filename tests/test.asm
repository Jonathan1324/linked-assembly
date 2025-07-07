[BITS 32]

section .text
    global _start

_start:
    mov eax, 1
    mov ebx, 0
    int 0x80

section .data
data1:  db 0
data2   dw 2