section .text
    global _start

_start:
    mov eax, 4

section .data

extern labelExtern1

label1:         dd 0x12345678
label2:         dd 0x87654321