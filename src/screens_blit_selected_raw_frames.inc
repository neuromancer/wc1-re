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
        mov	esi, dword ptr [ebp + 0x18]
        mov	edi, dword ptr [ebp + 0xc]
dormant_loc_43e64e:
        movzx	eax, byte ptr [esi]
        push	dword ptr [ebp + 0x1c]
        push	eax
        push	dword ptr [ebp + 0x14]
        push	dword ptr [ebp + 0x10]
        push	edi
        push	dword ptr [ebp + 0x8]
        call BlitRawFrame
        add	esp, 0x18
        add	edi, eax
        inc	esi
        cmp	byte ptr [esi], 0x0
        jne dormant_loc_43e64e
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
