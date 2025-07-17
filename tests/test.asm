bits 32

section .text
    global _start

_start:
    mov eax, ebx
    mov ebx, 0x35 +$

section .data

msg1    db "Hello, world!"
msg2    dw "Hello, world!"
msg3    dd "Hello, world!"
msg4    dq "Hello, world!"

msg5 db 3 + 2