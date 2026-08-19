    __asm {
        mov	edi, edi
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
        call	dword ptr [esi]
        mov edi, offset szRasterDriverName
dormant_loc_439c54:
        mov	bl, byte ptr [eax]
        mov	byte ptr [edi], bl
        inc	edi
        inc	eax
        or	bl, bl
        jne dormant_loc_439c54
        mov eax, offset szRasterDriverName
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
