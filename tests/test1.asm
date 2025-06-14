section .text
    global _start

_start:
    mov eax, [data2]

section .data

data1:          dq 0x0123456789abcdef
data2:          dd 0x11111111