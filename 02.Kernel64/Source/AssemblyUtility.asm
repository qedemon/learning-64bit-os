[BITS 64]
global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTSSR, kLoadIDTR
SECTION .text
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
kLoad:
    ltr di
    ret
kLoadIDTR:
    lidt [rdi]
    ret