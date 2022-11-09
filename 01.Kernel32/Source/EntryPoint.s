[ORG 0x00]
[BITS 16]
SECTION .text

START:
    mov ax, 0x1000
    mov ds, ax
    mov es, ax

    mov ax, 0x2401
    int 0x15
    jc .A20GATEERROR
    jmp .A20GATESUCCESS
    .A20GATEERROR:
    in al, 0x92
    or al, 0x02
    and al, 0xFE
    out 0x92, al
    .A20GATESUCCESS:

    cli

    lgdt[GDTR]

    mov eax, 0x4000003B
    mov cr0, eax
    jmp dword (CODEDESCRIPTOR-GDT):(0x10000+PROTECTEDMODE-$$)

[BITS 32]
PROTECTEDMODE:
    mov ax, (DATADESCRIPTOR-GDT)
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0xFFFC
    mov esp, 0xFFFC

    push (MODESWITCHMESSAGE-$$+0x10000)
    push 2
    push 0
    call PRINTMESSAGE
    add esp, 12

    

    jmp dword (CODEDESCRIPTOR-GDT):0x10200

PRINTMESSAGE:
    push ebp
    mov ebp, esp
    push eax
    push ecx
    push esi
    push edi

    mov edi, 0xB8000
    mov eax, 160
    mov esi, dword[ebp+12]
    mul esi
    add edi, eax

    mov eax, 2
    mov esi, dword[ebp+8]
    mul esi
    add edi, eax

    mov esi, dword[ebp+16]

    .PRINTMESSAGELOOP:
        mov cl, byte[esi]
        cmp cl, 0
        je .PRINTMESSAGELOOPEND
        mov byte[edi], cl
        add edi, 2
        add esi, 1
        jmp .PRINTMESSAGELOOP
    .PRINTMESSAGELOOPEND:

    pop edi
    pop esi
    pop ecx
    pop eax
    pop ebp
    ret

align 8, db 0x00
dw 0x0000
GDTR:
    dw (GDTEND-GDT-1)
    dd (GDT-$$+0x10000)
GDT:
    NULLDESCRIPTOR:
        dd 0x00000000
        dd 0x00000000
    IA_32eCODEDESCRIPTOR:
        dw 0xffff;  세그먼트크기[15:0]
        dw 0x0000;  기준주소[15:0]
        db 0x00;    기준주소[23:16]
        db 0x9A;    타입 실행/읽기 S=1 DPL=0 P=1
        db 0xAF;    세그먼트크기[19:16] AVL=0 L=1 D/B=0 G=1
        db 0x00;    기준주소[31:24]
    IA_32eDATADESCRIPTOR:
        dw 0xffff;  세그먼트크기[15:0]
        dw 0x0000;  기준주소[15:0]
        db 0x00;    기준주소[23:16]
        db 0x92;    타입 쓰기/읽기 S=1 DPL=0 P=1
        db 0xAF;    세그먼트크기[19:16] AVL=0 L=1 D/B=0 G=1
        db 0x00;    기준주소[31:24]
    CODEDESCRIPTOR:
        dw 0xffff;  세그먼트크기[15:0]
        dw 0x0000;  기준주소[15:0]
        db 0x00;    기준주소[23:16]
        db 0x9A;    타입 실행/읽기 S=1 DPL=0 P=1
        db 0xCF;    세그먼트크기[19:16] AVL=0 L=0 D/B=1 G=1
        db 0x00;    기준주소[31:24]
    DATADESCRIPTOR:
        dw 0xffff;  세그먼트크기[15:0]
        dw 0x0000;  기준주소[15:0]
        db 0x00;    기준주소[23:16]
        db 0x92;    타입 쓰기/읽기 S=1 DPL=0 P=1
        db 0xCF;    세그먼트크기[19:16] AVL=0 L=0 D/B=1 G=1
        db 0x00;    기준주소[31:24]
GDTEND:

MODESWITCHMESSAGE: db 'Switch to the Protected Mode Success~!!',0

times 512-($-$$) db 0x00