; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    int 0x80
    iret
    iretq
    iretd
    syscall
    sysret

    sysenter
    sysexit

