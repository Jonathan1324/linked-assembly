[bits 32]

section .text
    global _start

_start:
    ; sys_creat (fd = EAX)
    mov eax, 8          ; sys_creat
    mov ebx, filename   ; Filename
    mov ecx, 0666       ; Permissions
    int 0x80

    db 0x89, 0xC3
    ; TODO: mov reg, reg
    ;mov ebx, eax        ; fd in ebx speichern

    ; sys_write(fd, buf, count)
    mov eax, 4          ; sys_write
    mov ecx, content
    mov edx, len
    int 0x80

    ; sys_close(fd)
    mov eax, 6          ; sys_close
    int 0x80

    ; sys_exit(0) ohne xor
    mov eax, 1          ; sys_exit
    mov ebx, 0          ; Exit-Code 0
    int 0x80

section .data
    filename db "test.txt", 0
    content  db "Hello from ASM!", 0xA
    len      equ $ - content
