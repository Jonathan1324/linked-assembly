[bits 32]

section .text
    global _start

_start:
    mov eax, tr7
    db 0xFF
    mov tr0, eax
