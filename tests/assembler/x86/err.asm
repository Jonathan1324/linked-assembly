[bits 32]

%include "tests/assembler/x86/test.asm"

section .text
    global _start

_start:
    mov 5, ax
