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
        add	esi, 0x8
        mov	eax, dword ptr [ebp + 0xc]
        shl	eax, 0x3
        add	esi, eax
        add	esi, 0x4
        mov	esi, dword ptr [esi]
        cmp	esi, 0x0
        jne dormant_loc_43eff2
        mov	eax, 0x0
        jmp dormant_loc_43f00b
dormant_loc_43eff2:
        add	esi, dword ptr [ebp + 0x8]
        lodsd
        mov	ebx, eax
        mov	edi, dword ptr [ebp + 0x10]
        or	edi, edi
        je dormant_loc_43f00b
        mov	ecx, ebx
        mov	eax, 0x0
dormant_loc_43f006:
        movsd
        loop dormant_loc_43f006
        mov	eax, ebx
dormant_loc_43f00b:
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
