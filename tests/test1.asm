section .text
    global _start

_start:
    mov eax, [buffer]
    ret

section .bss
buffer:         resb 0x200