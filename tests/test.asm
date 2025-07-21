bits 32

section .text
    global _start

_start:
    mov eax, 0x10
    mov ebx, 0x00


testConst equ $ + 1

section .data

msg db "Hello, World!"