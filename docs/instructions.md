# Supported Instructions

This is the current set of supported x86 instructions in 32-bit mode:

### Control Flow & CPU
- `nop`
- `ret`
- `ret imm16`
- `int3`
- `int1`
- `hlt`
- `pause`
- `wait` / `fwait`

### Flags & Interrupts
- `cli` / `sti`
- `clc` / `stc` / `cmc`
- `std`

### CPU Info
- `cpuid`
- `rdtsc`

### MOV
- `mov reg, imm32`
- `mov reg, reg`
