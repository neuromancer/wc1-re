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
        push offset abIffBmhdChunkId
        call FindIFFChunkData
        add esp, 8
        mov esi, eax
        lodsw
        xchg ah, al
        shl eax, 10h
        lodsw
        xchg ah, al
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
