    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0x10
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov edi, offset abRasterPaletteUsageScratch
        mov	eax, 0x0
        mov	ecx, 0x40
        rep		stosd
        mov	esi, dword ptr [ebp + 0x8]
        mov	eax, dword ptr [esi + 0x4]
        inc	eax
        mov	ecx, dword ptr [esi + 0x8]
        inc	ecx
        imul	ecx, eax
        mov	esi, dword ptr [esi]
        mov edi, offset abRasterDecodeBuffer + 0x300
        mov	dword ptr [ebp - 0x4], 0xffffffff
dormant_loc_43f45f:
        mov	eax, 0x0
        lodsb
        cmp byte ptr abRasterPaletteUsageScratch[eax], 0x0
        jne dormant_loc_43f492
        or byte ptr abRasterPaletteUsageScratch[eax], 0x1
        stosb
        inc	dword ptr [ebp - 0x4]
        mov	ebx, eax
        shl	ebx, 1
        add	ebx, eax
        add ebx, offset abRasterDecodeBuffer
        push	ecx
        push	ebx
        push	eax
        call	dword ptr [apRasterDriverCallbacks + 0x20]
        add	esp, 0x8
        pop	ecx
dormant_loc_43f492:
        dec	ecx
        jne dormant_loc_43f45f
        mov	dword ptr [ebp - 0x8], 0x0
        mov	ecx, dword ptr [ebp - 0x4]
        mov	ebx, 0x0
        mov	esi, dword ptr [ebp + 0xc]
dormant_loc_43f4a7:
        movzx eax, byte ptr abRasterDecodeBuffer[ecx + 0x300]
        mov	ebx, eax
        shl	ebx, 1
        add	ebx, eax
        mov	edx, 0x2
dormant_loc_43f4b9:
        mov	ah, -0x1
        mov al, byte ptr abRasterDecodeBuffer[ebx]
        sub	al, byte ptr [ebx + esi]
        jge dormant_loc_43f4ca
        mov	ah, 0x1
        neg	al
dormant_loc_43f4ca:
        mov byte ptr abRasterPaletteUsageScratch[ebx], ah
        mov byte ptr abRasterDecodeBuffer[ebx + 0x400], al
        cmp	al, byte ptr [ebp - 0x8]
        jle dormant_loc_43f4de
        mov	byte ptr [ebp - 0x8], al
dormant_loc_43f4de:
        inc	ebx
        dec	edx
        jns dormant_loc_43f4b9
        dec	ecx
        jns dormant_loc_43f4a7
        mov	ecx, dword ptr [ebp - 0x4]
        mov edi, offset abRasterFadeAccumulator
        mov	al, byte ptr [ebp - 0x8]
        shr	al, 1
        mov	ah, al
        shr	ecx, 1
        rep		stosw
        adc	ecx, 0x0
        rep		stosb
        movzx	esi, byte ptr [ebp - 0x8]
        cmp	esi, 0x0
        je dormant_loc_43f5a3
        mov	eax, dword ptr [ebp + 0x10]
        mov	edx, 0x0
        shld	edx, eax, 0x10
        shl	eax, 0x10
        div	esi
        mov	dword ptr [ebp - 0xc], eax
        mov	dword ptr [ebp - 0x10], 0x8000
dormant_loc_43f526:
        mov	ecx, dword ptr [ebp - 0x4]
dormant_loc_43f529:
        movzx eax, byte ptr abRasterDecodeBuffer[ecx + 0x300]
        mov	ebx, eax
        shl	ebx, 1
        add	ebx, eax
        mov	edx, 0x2
dormant_loc_43f53b:
        mov al, byte ptr abRasterFadeAccumulator[edx + ebx]
        add al, byte ptr abRasterDecodeBuffer[edx + ebx + 0x400]
        cmp	al, byte ptr [ebp - 0x8]
        jl dormant_loc_43f55f
        sub	al, byte ptr [ebp - 0x8]
        mov ah, byte ptr abRasterPaletteUsageScratch[edx + ebx]
        add byte ptr abRasterDecodeBuffer[edx + ebx], ah
dormant_loc_43f55f:
        mov byte ptr abRasterFadeAccumulator[edx + ebx], al
        dec	edx
        jns dormant_loc_43f53b
        push	ecx
        mov	eax, ebx
        add eax, offset abRasterDecodeBuffer
        push	eax
        movzx eax, byte ptr abRasterDecodeBuffer[ecx + 0x300]
        push	eax
        call	dword ptr [apRasterDriverCallbacks + 0x24]
        add	esp, 0x8
        pop	ecx
        dec	ecx
        jns dormant_loc_43f529
        mov	eax, dword ptr [ebp - 0xc]
        add	dword ptr [ebp - 0x10], eax
        cmp	word ptr [ebp - 0xe], 0x1
        jl dormant_loc_43f5a0
dormant_loc_43f594:
        call	dword ptr [apRasterDriverCallbacks + 0x14]
        dec	word ptr [ebp - 0xe]
        jne dormant_loc_43f594
dormant_loc_43f5a0:
        dec	esi
        jne dormant_loc_43f526
dormant_loc_43f5a3:
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
