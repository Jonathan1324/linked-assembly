[bits 32]

section .text
extern label2

global _start

_start:
    mov eax, testExtern
    mov ecx, label2
    mov ax, _start

extern testExtern
