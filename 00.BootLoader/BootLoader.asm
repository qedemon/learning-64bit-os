[ORG 0x00]
[BITS 16]

SECTION .text
jmp 0x07C0:START
TOTALSECTORCOUNT: dw 1024
KERNEL32SECTORCOUNT: dw 512

START:
    mov ax, 0x0000
    mov ss, ax
    mov ax, 0xFFFE
    mov bp, ax
    mov sp, ax

    mov ax, 0x07C0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    SCREENCLEAR:
        mov ax, 0xB800
        mov es, ax
        mov di, 0x0000

        .SCREENCLEARLOOP:
            cmp di, 80*25*2
            jge .SCREENCLEAREND

            mov byte[es:di], 0
            mov byte[es:di+1], 0x1F

            add di, 0x02
            jmp .SCREENCLEARLOOP
    .SCREENCLEAREND:

    push MESSAGE1
    push 0
    push 0
    call PRINTMESSAGE
    add sp, 6

    push OSIMAGELOADEINGMESSAGE
    push 1
    push 0
    call PRINTMESSAGE
    add sp, 6

    RESETDISK:
    mov ah, 0x00
    mov dl, 0x00
    int 0x13
    jc HANDLEDISKERROR

    push word[TOTALSECTORCOUNT]
    push 0x1000
    call READDISK
    add sp, 4

    push LOADINGCOMPLETEMESSAGE
    push 1
    push 20
    call PRINTMESSAGE
    add sp, 6

    jmp 0x1000:0000

;함수 영역

HANDLEDISKERROR:
    push DISKERRORMESSAGE
    push 1
    push 20
    call PRINTMESSAGE

    jmp $

READDISK:
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push es
    push di
    push si

    mov si, word[bp+4]  ;메모리에 저장할 주소
    mov di, word[bp+6]  ;읽을 섹터 수

    .READDISKLOOP:
        cmp di, 0
        je .READDISKLOOPEND

        mov ah, 0x02
        mov al, 0x01
        mov ch, byte[TRACKNUMBER]
        mov cl, byte[SECTORNUMBER]
        mov dh, byte[HEADNUMBER]
        mov dl, 0x00
        mov es, si
        mov bx, 0x00
        int 0x13
        jc HANDLEDISKERROR

        sub di, 1

        add si, 0x0020
        mov es, si

        add byte[SECTORNUMBER], 0x01
        cmp byte[SECTORNUMBER], 19
        jl .READDISKLOOP

        mov byte[SECTORNUMBER], 0x01
        xor byte[HEADNUMBER], 0x01
        cmp byte[HEADNUMBER], 1
        je .READDISKLOOP

        add byte[TRACKNUMBER], 0x01

        jmp .READDISKLOOP
    .READDISKLOOPEND:

    pop di
    pop si
    pop es
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
    ret

PRINTMESSAGE:
    push bp
    mov bp, sp
    push ax
    push cx
    push di
    push si
    push es

    mov ax, word[bp+4]  ;X 값 불러오기
    mov si, 2
    mul si
    mov di, ax

    mov ax, word[bp+6]  ;Y값 불러오기
    mov si, 160
    mul si
    add di, ax

    mov si, word[bp+8]  ;message포인터 불러오기
    mov ax, 0xB800
    mov es, ax

    .PRINTMESSAGELOOP:
        mov cl, byte[si]
        cmp cl, 0x00
        je .PRINTMESSAGELOOPEND
        mov byte[es:di], cl
        add di, 2
        add si, 1
        jmp .PRINTMESSAGELOOP
    .PRINTMESSAGELOOPEND:

    pop es
    pop si
    pop di
    pop cx
    pop ax
    pop bp
    ret


;데이터 영역
SECTORNUMBER:   db 0x02
TRACKNUMBER: db 0x00
HEADNUMBER: db 0x00

MESSAGE1: db 'MINT64 OS BootLoader Start~!', 0
DISKERRORMESSAGE: db 'Disk Error~!!', 0
OSIMAGELOADEINGMESSAGE: db 'OS Image Loading.....', 0
LOADINGCOMPLETEMESSAGE: db 'Complete~!!', 0


times 510-($-$$) db 0x00
db 0x55
db 0xAA