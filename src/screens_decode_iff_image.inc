    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0x3c
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov	edi, dword ptr [ebp + 0x8]
        mov	eax, dword ptr [edi + 0xc]
        sub	eax, dword ptr [edi + 0x4]
        inc	eax
        mov	dword ptr [ebp - 0x24], eax
        mov	eax, dword ptr [edi + 0x10]
        sub	eax, dword ptr [edi + 0x8]
        inc	eax
        mov	dword ptr [ebp - 0x20], eax
        mov	edi, dword ptr [edi]
        mov	edi, dword ptr [edi]
        mov	dword ptr [ebp - 0x3c], edi
        mov	dword ptr [ebp - 0x28], 0x0
        mov	edi, dword ptr [ebp + 0xc]
        mov	eax, dword ptr [edi + 0x8]
        xor	eax, 0x4d424c49
        mov	dword ptr [ebp - 0x4], eax
        push	dword ptr [ebp + 0xc]
        push offset g_abIffBmhdChunkId_0043e778
        call FindIFFChunkData
        add	esp, 0x8
        mov	esi, eax
        lodsw
        xchg	al, ah
        and	eax, 0xffff
        mov	dword ptr [ebp - 0xc], eax
        lodsw
        xchg	al, ah
        and	eax, 0xffff
        cmp	eax, dword ptr [ebp - 0x20]
        jl dormant_loc_43e835
        mov	eax, dword ptr [ebp - 0x20]
dormant_loc_43e835:
        mov	dword ptr [ebp - 0x8], eax
        add	esi, 0x5
        lodsb
        cmp	al, 0x1
        je dormant_loc_43e984
        mov	eax, 0x0
        lodsb
        mov	dword ptr [ebp - 0x2c], eax
        add	esi, 0x2
        mov	eax, 0x0
        lodsb
        mov	dword ptr [ebp - 0x38], eax
        mov	eax, dword ptr [ebp - 0xc]
        mov	ebx, eax
        shr	eax, 0x3
        and	ebx, 0x7
        cmp	ebx, 0x1
        sbb	eax, -0x1
        mov	ebx, eax
        and	eax, 0x1
        add	ebx, eax
        mov	dword ptr [ebp - 0x30], ebx
        mov	eax, dword ptr [ebp - 0xc]
        and	eax, 0x1
        add	eax, dword ptr [ebp - 0xc]
        mov	dword ptr [ebp - 0x34], eax
        mov	eax, dword ptr [ebp - 0xc]
        cmp	eax, dword ptr [ebp - 0x24]
        jl dormant_loc_43e88b
        mov	eax, dword ptr [ebp - 0x24]
dormant_loc_43e88b:
        mov	dword ptr [ebp - 0xc], eax
        push	dword ptr [ebp + 0xc]
        push offset g_abIffBodyChunkId_0043e780
        call FindIFFChunkData
        add	esp, 0x8
        mov	dword ptr [ebp - 0x10], eax
dormant_loc_43e8a1:
        mov	esi, dword ptr [ebp - 0x10]
        cmp	dword ptr [ebp - 0x2c], 0x1
        jne dormant_loc_43e8fc
        mov edi, offset g_abRasterPaletteUsageScratch_0046f915
        mov	edx, dword ptr [ebp - 0x34]
        add	edx, edi
dormant_loc_43e8b4:
        cmp	edi, edx
        jae dormant_loc_43e8f2
        lodsb
        movzx	ecx, al
        cmp	ecx, 0x80
        je dormant_loc_43e8b4
        ja dormant_loc_43e8d5
        inc	ecx
        push	ecx
        and	ecx, 0x3
        rep		movsb
        pop	ecx
        shr	ecx, 0x2
        rep		movsd
        jmp dormant_loc_43e8b4
dormant_loc_43e8d5:
        lodsb
        mov	ah, al
        mov	ebx, eax
        shl	eax, 0x10
        mov	ax, bx
        neg	cl
        inc	cl
        push	ecx
        and	ecx, 0x3
        rep		stosb
        pop	ecx
        shr	ecx, 0x2
        rep		stosd
        jmp dormant_loc_43e8b4
dormant_loc_43e8f2:
        mov	dword ptr [ebp - 0x10], esi
        mov esi, offset g_abRasterPaletteUsageScratch_0046f915
        jmp dormant_loc_43e904
dormant_loc_43e8fc:
        mov	eax, esi
        add	eax, dword ptr [ebp - 0x34]
        mov	dword ptr [ebp - 0x10], eax
dormant_loc_43e904:
        cmp	dword ptr [ebp - 0x4], 0x0
        jne dormant_loc_43e966
        mov edi, offset g_abRasterDecodeBuffer_0046ec15
        mov	eax, dword ptr [ebp - 0x30]
        mov	dword ptr [ebp - 0x18], eax
        mov	dword ptr [ebp - 0x1c], eax
        mov	eax, dword ptr [ebp - 0xc]
        mov	dword ptr [ebp - 0x14], eax
dormant_loc_43e91e:
        mov	edx, 0x80
dormant_loc_43e923:
        mov	ebx, 0x0
        mov	eax, 0x100
dormant_loc_43e92d:
        movzx	ecx, byte ptr [esi + ebx]
        and	ecx, edx
        je dormant_loc_43e937
        or	al, ah
dormant_loc_43e937:
        add	ebx, dword ptr [ebp - 0x1c]
        shl	ah, 1
        jne dormant_loc_43e92d
        stosb
        dec	dword ptr [ebp - 0x14]
        je dormant_loc_43e94e
        shr	dl, 1
        jne dormant_loc_43e923
        inc	esi
        dec	dword ptr [ebp - 0x18]
        jne dormant_loc_43e91e
dormant_loc_43e94e:
        push	dword ptr [ebp - 0xc]
        push offset g_abRasterDecodeBuffer_0046ec15
        push	dword ptr [ebp - 0x28]
        push	dword ptr [ebp + 0x8]
        call BlitRawScanline
        add	esp, 0x10
        jmp dormant_loc_43e978
dormant_loc_43e966:
        push	dword ptr [ebp - 0xc]
        push	esi
        push	dword ptr [ebp - 0x28]
        push	dword ptr [ebp + 0x8]
        call BlitRawScanline
        add	esp, 0x10
dormant_loc_43e978:
        inc	dword ptr [ebp - 0x28]
        dec	dword ptr [ebp - 0x8]
        jne dormant_loc_43e8a1
dormant_loc_43e984:
        mov	eax, dword ptr [ebp - 0x38]
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
