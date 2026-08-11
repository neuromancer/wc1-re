    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        add esi, dword ptr [ebp + 0ch]
        sub esi, 300h
        mov edi, dword ptr [ebp + 10h]
        mov ecx, 300h
copy_pcx_palette_component:
        lodsb
        shr al, 2
        stosb
        loop copy_pcx_palette_component
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
