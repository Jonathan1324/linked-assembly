global cpu
cpu:
    mov eax, 0
    cpuid
    ret