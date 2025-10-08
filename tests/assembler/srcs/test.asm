[bits 32]

section .text
    global _start

_start:
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov ecx, buffer     ; Pufferadresse
    mov edx, 128        ; max Länge
    int 0x80
    mov esi, eax        ; Anzahl gelesener Bytes merken

    ; --- "Input: " ausgeben ---
    mov eax, 4          ; sys_write
    mov ebx, 1          ; stdout
    mov ecx, prefix
    mov edx, prefix_len
    int 0x80

    ; --- Eingabe ausgeben ---
    mov eax, 4          ; sys_write
    mov ebx, 1          ; stdout
    mov ecx, buffer
    mov edx, esi        ; Anzahl gelesener Bytes
    int 0x80

    ; --- Programm beenden ---
    mov eax, 1          ; sys_exit
    mov ebx, 0
    int 0x80

section .data
    prefix db "Input: "
    prefix_len equ $ - prefix

    ; resb not implemented
    buffer dq 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
           dq 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
