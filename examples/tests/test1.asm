section .text
    global _start

_start:
    mov eax, 0x12345678
    mov ebx, eax
    mov edx, ecx
    mov ecx, edx
    int3
    mov eax, 0