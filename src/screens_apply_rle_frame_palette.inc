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
        je dormant_loc_43efc3
        add	esi, dword ptr [ebp + 0x8]
        lodsd
        mov	ecx, eax
        mov	edi, dword ptr [ebp + 0x10]
dormant_loc_43efa9:
        lodsb
        and	eax, 0xff
        mov	ebx, eax
        shl	ebx, 1
        add	ebx, eax
        lodsb
        mov	byte ptr [ebx + edi], al
        inc	ebx
        lodsw
        mov	word ptr [ebx + edi], ax
        dec	ecx
        jne dormant_loc_43efa9
dormant_loc_43efc3:
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
