[bits 32]

extern label

section .text
extern label2
global _start

_start:
    mov eax, label
