[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x1000:START

TOTALSECTORCOUNT equ 1024

START:
    mov ax, cs
    mov ds, ax
    mov ax, 0xB800
    mov es, ax

    %assign i 0
    %rep TOTALSECTORCOUNT
        mov si, 160*2+(i*2)
        %assign i i+1
        
        mov byte[es:si], (i % 10)+'0'

        %if i==TOTALSECTORCOUNT
            jmp $
        %else
            jmp (0x1000+i*0x20):0000
        %endif

        times (512-($-$$) % 512) db 0x00
    %endrep
