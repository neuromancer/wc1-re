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
        mov esi, dword ptr [ebp + 0ch]
        add esi, 0ch
find_iff_chunk_nonzero:
        cmp byte ptr [esi], 0
        jne compare_iff_chunk_id
        inc esi
        jmp find_iff_chunk_nonzero
compare_iff_chunk_id:
        mov ecx, 2
        mov edi, dword ptr [ebp + 8]
        mov eax, esi
        repe cmpsw
        je found_iff_chunk
        mov esi, eax
        add esi, 6
        lodsw
        xchg ah, al
        and eax, 0ffffh
        add esi, eax
        jmp find_iff_chunk_nonzero
found_iff_chunk:
        add eax, 8
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
