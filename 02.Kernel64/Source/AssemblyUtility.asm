[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTSSR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS
global kReadTSC
global kGetRIPValue
global kHlt
global kTestAndSet

kInPortByte:
    push rdx
    mov rdx, rdi
    mov rax, 0
    in al, dx
    pop rdx
    ret
kOutPortByte:
    push rdx
    mov rdx, rdi
    mov rax, rsi
    out dx, al
    pop rdx
    ret
kLoadGDTR:
    lgdt [rdi]
    ret
kLoadTSSR:
    ltr di
    ret
kLoadIDTR:
    lidt [rdi]
    ret

kEnableInterrupt:
    sti
    ret
kDisableInterrupt:
    cli
    ret
kReadRFLAGS:
    pushfq
    pop rax
    ret
kReadTSC:
    push rdx
    rdtsc
    shl rdx, 32
    or rax, rdx
    pop rdx
    ret
kHlt:
    hlt
    ret
kTestAndSet:
    mov rax, rsi
    lock cmpxchg byte[rdi], dl
    je .SUCCESS
.NOTSAME:
    mov rax, 0
    ret
.SUCCESS:
    mov rax, 1
    ret