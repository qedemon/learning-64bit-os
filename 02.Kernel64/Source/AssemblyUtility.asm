[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte, kInPortWord, kOutPortWord, kLoadGDTR, kLoadTSSR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS
global kReadTSC
global kGetRIPValue
global kHlt
global kTestAndSet
global kInitializeFPU, kSaveFPUContext, kLoadFPUContext, kSetTSBit, kClearTSBit

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
kInPortWord:
    push rdx
    mov rdx, rdi
    mov rax, 0
    in ax, dx
    pop rdx
    ret
kOutPortWord:
    push rdx
    push rax
    mov rdx, rdi
    mov rax, rsi
    out dx, ax
    pop rax
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
kInitializeFPU:
    finit
    ret
kSaveFPUContext:
    fxsave [rdi]
    ret
kLoadFPUContext:
    fxrstor [rdi]
    ret
kSetTSBit:
    mov rax, cr0
    or rax, 0x08
    mov cr0, rax
    ret
kClearTSBit:
    clts
    ret