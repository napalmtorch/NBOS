[GLOBAL thread_switch]
[EXTERN THREAD]
[EXTERN THREAD_NEXT]

thread_switch:
    mov EAX, [THREAD]

    mov [EAX+0],  ESP
    mov [EAX+4],  EBP
    mov [EAX+8],  ESI
    mov [EAX+12], EDI
    mov [EAX+16], EAX
    mov [EAX+20], EBX
    mov [EAX+24], ECX
    mov [EAX+28], EDX

    pushf
    pop ECX
    mov [EAX+32], ECX

    mov EAX, [THREAD_NEXT]

    mov ECX, [EAX+32]
    push ECX
    popf

    mov ESP, [EAX+0]
    mov EBP, [EAX+4]
    mov ESI, [EAX+8]
    mov EDI, [EAX+12]
    mov EBX, [EAX+20]
    mov ECX, [EAX+24]
    mov EDX, [EAX+28]

    mov [THREAD], EAX
    mov EAX, [EAX+16]
    
    sti
    ret