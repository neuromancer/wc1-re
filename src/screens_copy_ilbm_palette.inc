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
        push dword ptr [ebp + 8]
        push offset g_abIffCmapChunkId_0043e77c
        call FindIFFChunkData
        add esp, 8
        mov esi, eax
        mov edi, dword ptr [ebp + 0ch]
        mov ecx, 300h
copy_ilbm_palette_component:
        lodsb
        shr al, 2
        stosb
        loop copy_ilbm_palette_component
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
