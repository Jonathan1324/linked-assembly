; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: ERROR

[bits 32]

section .text
    global _start

_start:
    mov 5, ax
