section .text
    global _start

_start:
    mov eax, 0x12345678
    mov ebx, eax
    mov edx, ecx
    mov ecx, ebx
    int3
    mov eax, 0
.next:
    mov ebx, 0x20
.done:
    ret

section .data

data1:          dq 0x0123456789abcdef
data2:          dw 0xFEED