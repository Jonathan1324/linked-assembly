bits 32

section .text
    global _start

_start:
    mov eax, 1       ; syscall number (sys_exit)
    mov ebx, 0; exit code 0
    int 0x80         ; interrupt to invoke syscall