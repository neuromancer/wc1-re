    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0xc
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov	dword ptr [ebp - 0xc], 0x0
        mov	edi, dword ptr [ebp + 0x8]
        mov	edi, dword ptr [edi]
        mov	edi, dword ptr [edi]
        mov	esi, dword ptr [ebp + 0xc]
        movzx	ebx, word ptr [esi + 0xa]
        sub	bx, word ptr [esi + 0x6]
        mov	dword ptr [ebp - 0x8], ebx
        mov	ebx, 0x0
        movzx	eax, word ptr [esi + 0x42]
        mov	dword ptr [ebp - 0x4], eax
        add	esi, 0x80
dormant_loc_43ea26:
        mov edi, offset abRasterDecodeBuffer
        mov	edx, edi
        add	edx, dword ptr [ebp - 0x4]
dormant_loc_43ea30:
        lodsb
        mov	ah, al
        and	ah, -0x40
        xor	ah, -0x40
        jne dormant_loc_43ea45
        and	eax, 0x3f
        mov	ecx, eax
        lodsb
        rep		stosb
        jmp dormant_loc_43ea46
dormant_loc_43ea45:
        stosb
dormant_loc_43ea46:
        cmp	edi, edx
        jl dormant_loc_43ea30
        push	dword ptr [ebp - 0x4]
        push offset abRasterDecodeBuffer
        push	ebx
        push	dword ptr [ebp + 0x8]
        call BlitRawScanline
        add	esp, 0x10
        inc	ebx
        cmp	ebx, dword ptr [ebp - 0x8]
        jle dormant_loc_43ea26
        mov	eax, dword ptr [ebp - 0xc]
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
