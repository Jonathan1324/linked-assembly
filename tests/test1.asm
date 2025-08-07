section .text
global _start

_start:
    ;mov eax, 1       ; syscall number (sys_exit)
    ;mov ebx, 0; exit code 0
    int 0x80         ; interrupt to invoke syscall

section .data
data1:
    db 0

data2: dw 2

msg db "Hello, World!"
testChar:   db 'a'