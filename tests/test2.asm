section .text
    global test

test:
    nop
.nextStep:
    mov eax, 0
.anotherStep:
    mov ebx, 1
.done:
    ret