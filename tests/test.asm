[BITS 32]

section .text
global _start
_start:
    mov eax, 1
    mov ebx, 42
    jmp intf

global testf
testf:
    mov ebx, 1

global intf
intf:
    int 0x80