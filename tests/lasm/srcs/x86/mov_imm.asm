; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

section .data
    global _start

_start:
    mov eax, 0
