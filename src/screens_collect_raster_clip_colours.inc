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
        mov edi, offset g_abRasterPaletteUsageScratch_0046f915
        mov	eax, 0x0
        mov	ecx, 0x40
        rep		stosd
        mov	esi, dword ptr [ebp + 0x8]
        mov	ecx, dword ptr [esi + 0xc]
        sub	ecx, dword ptr [esi + 0x4]
        mov	dword ptr [ebp - 0x8], ecx
        mov	ebx, dword ptr [esi + 0x10]
        sub	ebx, dword ptr [esi + 0x8]
        mov	esi, dword ptr [esi]
        mov	eax, dword ptr [esi + 0x4]
        inc	eax
        mov	dword ptr [ebp - 0xc], eax
        mov	esi, dword ptr [ebp + 0x8]
        mov	eax, dword ptr [esi + 0x8]
        imul	dword ptr [ebp - 0xc]
        add	eax, dword ptr [esi + 0x4]
        mov	esi, dword ptr [esi]
        mov	esi, dword ptr [esi]
        add	esi, eax
        mov	edi, dword ptr [ebp + 0xc]
        mov	dword ptr [ebp - 0x4], 0xffffffff
dormant_loc_43f5fe:
        mov	eax, 0x0
        movzx	eax, byte ptr [ecx + esi]
        cmp byte ptr g_abRasterPaletteUsageScratch_0046f915[eax], 0x0
        jne dormant_loc_43f620
        or byte ptr g_abRasterPaletteUsageScratch_0046f915[eax], 0x1
        inc	dword ptr [ebp - 0x4]
        cmp	edi, 0x0
        je dormant_loc_43f620
        stosd
dormant_loc_43f620:
        dec	ecx
        jns dormant_loc_43f5fe
        mov	ecx, dword ptr [ebp - 0x8]
        add	esi, dword ptr [ebp - 0xc]
        dec	ebx
        jns dormant_loc_43f5fe
        mov	eax, dword ptr [ebp - 0x4]
        inc	eax
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
