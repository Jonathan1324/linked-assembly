section .text
    global _start

_start:
    mov [d], ss
    ret

section .bss
d:              resd 1