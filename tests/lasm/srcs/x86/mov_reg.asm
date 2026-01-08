; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    mov ax, bx
    mov eax, ebx
