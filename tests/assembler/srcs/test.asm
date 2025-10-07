[bits 64]

section .text
    global _start

_start:
    mov rax, dr15
    mov dr9, rax
