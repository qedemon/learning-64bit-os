[BITS 32]
global kReadCPUID, kCheck64BitMode, kSwitchAndExecute64BitMode

SECTION .text

kReadCPUID:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push edx
    push ecx
    push esi
    
    mov eax, 0x00000000
    cpuid
    mov esi, dword[ebp+8]
    mov dword[esi], ebx
    mov dword[esi+4], edx
    mov dword[esi+8], ecx

    pop esi
    pop ecx
    pop edx
    pop ebx
    pop eax
    pop ebp
    ret

kCheck64BitMode:
    push ebx
    push edx
    push ecx

    mov eax, 0x80000001
    cpuid
    mov eax, edx
    shr eax, 29
    and eax, 1

    pop ecx
    pop edx
    pop ebx
    ret

kSwitchAndExecute64BitMode:
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    mov eax, 0x100000
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x0100
    wrmsr
    
    mov eax, cr0
    or eax, 0xE0000000
    xor eax, 0x60000000
    mov cr0, eax

    jmp 0x08:0x200000
    jmp $