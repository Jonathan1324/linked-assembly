section .text
    global _start

_start:
    mov eax, 0
test:
    jmp test2
test3:
    mov ecx, eax


msg db "Hi \" Hi2"
c   db 'a'
c2  db '\n'