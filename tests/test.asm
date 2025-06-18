section .text
    global _start

_start:
    mov eax, [label1]
    mov ebx, [label2]

section .data

label1:         dd 0x12345678
label2:         dd 0x87654321