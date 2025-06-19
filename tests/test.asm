[BITS 32]

section .text
global _start
_start:
    mov eax, 0x12345678
    mov ebx, 1
    mov ecx, 2
    mov edx, 11
    mov esp, 143
    mov ebp, 25
    mov esi, 123456
    mov edi, 50
    
    mov ax, 0x0123
    mov bx, 0x4567
    mov cx, 0x89ab
    mov dx, 0xcdef
    mov sp, 1234
    mov bp, 1000
    mov si, 9999
    mov di, 5

    mov ah, 0x01
    mov al, 0x23
    mov bh, 0x45
    mov bl, 0x67
    mov ch, 0x89
    mov cl, 0xab
    mov dh, 0xcd
    mov dl, 0xef