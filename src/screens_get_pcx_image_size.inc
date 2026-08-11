    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp + 8]
        mov ax, word ptr [esi + 8]
        sub ax, word ptr [esi + 4]
        inc ax
        shl eax, 10h
        mov ax, word ptr [esi + 0ah]
        sub ax, word ptr [esi + 6]
        inc ax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
