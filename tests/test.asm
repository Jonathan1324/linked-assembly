[BITS 32]

section .text
global _start
_start:
    mov eax, 0x12345678
    mov ebx, 1
    mov ecx, 2
    mov edx, 11
    mov esp, 143
    mov ebp, 25
    mov esi, 123456
    mov edi, 50