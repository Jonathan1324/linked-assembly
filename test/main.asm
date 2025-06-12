section .text
    global _start

_start:
    xor eax, eax
.loop:
    inc eax
    cmp eax, 100
    jl .loop
.done:
    mov ebx, 0
    mov eax, 1
    int 0x80