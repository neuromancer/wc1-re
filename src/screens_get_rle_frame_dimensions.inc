    __asm {
        push	ebp
        mov	ebp, esp
        push	ebx
        push	esi
        push	edi
        push	es
        mov	esi, dword ptr [ebp + 0x8]
        add	esi, 0x8
        mov	eax, dword ptr [ebp + 0xc]
        shl	eax, 0x3
        add	esi, eax
        mov	esi, dword ptr [esi]
        add	esi, dword ptr [ebp + 0x8]
        mov	eax, dword ptr [esi]
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
