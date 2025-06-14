section .text
    global _start

_start:
    mov eax, 0x12345678
    mov ebx, eax
    mov edx, ecx
    mov ecx, ebx
    int3
    mov eax, 0

section .data

data1:          dq 0x0123456789abcdef
data2:          dw 0xFEED