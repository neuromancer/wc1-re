    __asm {
        push	ebp
        mov	ebp, esp
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov	esi, dword ptr [ebp + 0x8]
        mov edi, offset apRasterDriverCallbacks
        mov	ecx, 0xd
        rep		movsd
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
