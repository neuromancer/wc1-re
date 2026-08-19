    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0x38
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov	esi, dword ptr [ebp + 0x8]
        mov	ebx, dword ptr [esi]
        mov	eax, dword ptr [ebx + 0x4]
        inc	eax
        mov	dword ptr [ebp - 0x30], eax
        jle dormant_loc_43c20c
        mov	eax, dword ptr [ebx + 0x8]
        inc	eax
        mov	ecx, eax
        jle dormant_loc_43c20c
        mov	eax, dword ptr [esi + 0x4]
        mov	dword ptr [ebp - 0x34], eax
        cmp	eax, 0x0
        jg dormant_loc_43c1c0
        mov	eax, 0x0
dormant_loc_43c1c0:
        mov	dword ptr [ebp - 0x1c], eax
        mov	eax, dword ptr [esi + 0x8]
        mov	dword ptr [ebp - 0x38], eax
        cmp	eax, 0x0
        jg dormant_loc_43c1d3
        mov	eax, 0x0
dormant_loc_43c1d3:
        mov	dword ptr [ebp - 0x20], eax
        mov	eax, dword ptr [esi + 0xc]
        mov	edx, dword ptr [ebp - 0x30]
        dec	edx
        cmp	eax, edx
        jl dormant_loc_43c1e3
        mov	eax, edx
dormant_loc_43c1e3:
        mov	dword ptr [ebp - 0x24], eax
        mov	eax, dword ptr [esi + 0x10]
        mov	edx, ecx
        dec	edx
        cmp	eax, edx
        jl dormant_loc_43c1f2
        mov	eax, edx
dormant_loc_43c1f2:
        mov	dword ptr [ebp - 0x28], eax
        mov	eax, dword ptr [ebp - 0x24]
        cmp	eax, dword ptr [ebp - 0x1c]
        jl dormant_loc_43c217
        mov	eax, dword ptr [ebp - 0x28]
        cmp	eax, dword ptr [ebp - 0x20]
        jl dormant_loc_43c217
        mov	eax, dword ptr [ebx]
        mov	dword ptr [ebp - 0x2c], eax
        jmp dormant_loc_43c222
dormant_loc_43c20c:
        mov	eax, 0xffffffff
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
dormant_loc_43c217:
        mov	eax, 0xfffffffe
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
dormant_loc_43c222:
        mov	esi, dword ptr [ebp + 0x8]
        mov	ebx, dword ptr [esi]
        mov	edi, dword ptr [ebp + 0x18]
        mov	dword ptr [pRLEEncodeBuffer], edi
        or	edi, edi
        je dormant_loc_43c270
        mov	eax, dword ptr [esi + 0xc]
        sub	eax, dword ptr [esi + 0x4]
        shl	eax, 0x10
        mov	ax, word ptr [esi + 0x10]
        sub	ax, word ptr [esi + 0x8]
        mov	dword ptr [edi], eax
        add	edi, 0x4
        mov	eax, dword ptr [ebp + 0x10]
        shl	eax, 0x10
        mov	ax, word ptr [ebp + 0x14]
        mov	dword ptr [edi], eax
        add	edi, 0x4
        xor	eax, eax
        mov	dword ptr [edi], eax
        add	edi, 0x4
        mov	dword ptr [edi], eax
        add	edi, 0x4
        dec	eax
        mov	dword ptr [edi], eax
        add	edi, 0x4
        mov	dword ptr [edi], eax
        add	edi, 0x4
dormant_loc_43c270:
        mov	eax, dword ptr [ebp - 0x34]
        add	dword ptr [ebp + 0x10], eax
        mov	eax, dword ptr [ebp - 0x38]
        add	dword ptr [ebp + 0x14], eax
        mov	eax, dword ptr [ebp - 0x24]
        inc	eax
        sub	eax, dword ptr [ebp - 0x1c]
        mov	dword ptr [ebp - 0x8], eax
        mov	eax, dword ptr [ebp - 0x24]
        inc	eax
        sub	eax, dword ptr [ebp - 0x1c]
        mov	dword ptr [ebp - 0xc], eax
        mov	eax, 0x7fffffff
        mov	dword ptr [nRLEEncodedMinimumX], eax
        mov	dword ptr [nRLEEncodedMinimumY], eax
        neg	eax
        mov	dword ptr [nRLEEncodedMaximumX], eax
        mov	dword ptr [nRLEEncodedMaximumY], eax
        mov	eax, dword ptr [ebp - 0x20]
        imul	dword ptr [ebp - 0x30]
        add	eax, dword ptr [ebp - 0x2c]
        add	eax, dword ptr [ebp - 0x1c]
        mov	dword ptr [pRLEScanlineStart], eax
        mov	esi, eax
        mov	eax, dword ptr [ebp - 0x20]
        mov	dword ptr [ebp - 0x10], eax
        jmp dormant_loc_43c360
dormant_loc_43c2c9:
        mov	edi, dword ptr [pRLEScanlineStart]
        mov	dword ptr [pRLEScanCursor], edi
        mov	ecx, dword ptr [ebp - 0x8]
        mov	dword ptr [ebp - 0x14], ecx
        mov	al, byte ptr [ebp + 0xc]
        rep		scasb
        je dormant_loc_43c354
        mov	eax, dword ptr [nRLEEncodedMinimumY]
        cmp	eax, dword ptr [ebp - 0x10]
        jl dormant_loc_43c2ef
        mov	eax, dword ptr [ebp - 0x10]
dormant_loc_43c2ef:
        mov	dword ptr [nRLEEncodedMinimumY], eax
        mov	eax, dword ptr [nRLEEncodedMaximumY]
        cmp	eax, dword ptr [ebp - 0x10]
        jg dormant_loc_43c301
        mov	eax, dword ptr [ebp - 0x10]
dormant_loc_43c301:
        mov	dword ptr [nRLEEncodedMaximumY], eax
        mov	eax, dword ptr [ebp - 0x24]
        sub	eax, ecx
        cmp	eax, dword ptr [nRLEEncodedMinimumX]
        jl dormant_loc_43c318
        mov	eax, dword ptr [nRLEEncodedMinimumX]
dormant_loc_43c318:
        mov	dword ptr [nRLEEncodedMinimumX], eax
        mov	eax, dword ptr [pRLEScanlineStart]
        add	eax, dword ptr [ebp - 0x8]
        dec	eax
        mov	edi, eax
        mov	dword ptr [pRLEScanCursor], edi
        mov	ecx, dword ptr [ebp - 0x8]
        mov	dword ptr [ebp - 0x14], ecx
        mov	al, byte ptr [ebp + 0xc]
        std
        rep		scasb
        cld
        je dormant_loc_43c354
        mov	eax, dword ptr [ebp - 0x1c]
        add	eax, ecx
        cmp	eax, dword ptr [nRLEEncodedMaximumX]
        jg dormant_loc_43c34f
        mov	eax, dword ptr [nRLEEncodedMaximumX]
dormant_loc_43c34f:
        mov	dword ptr [nRLEEncodedMaximumX], eax
dormant_loc_43c354:
        mov	eax, dword ptr [ebp - 0x30]
        add	dword ptr [pRLEScanlineStart], eax
        inc	dword ptr [ebp - 0x10]
dormant_loc_43c360:
        mov	eax, dword ptr [ebp - 0x10]
        cmp	eax, dword ptr [ebp - 0x28]
        jle dormant_loc_43c2c9
        mov	edi, dword ptr [ebp + 0x18]
        or	edi, edi
        je dormant_loc_43c39f
        mov	eax, dword ptr [nRLEEncodedMinimumX]
        sub	eax, dword ptr [ebp + 0x10]
        mov	dword ptr [edi + 0x8], eax
        mov	eax, dword ptr [nRLEEncodedMinimumY]
        sub	eax, dword ptr [ebp + 0x14]
        mov	dword ptr [edi + 0xc], eax
        mov	eax, dword ptr [nRLEEncodedMaximumX]
        sub	eax, dword ptr [ebp + 0x10]
        mov	dword ptr [edi + 0x10], eax
        mov	eax, dword ptr [nRLEEncodedMaximumY]
        sub	eax, dword ptr [ebp + 0x14]
        mov	dword ptr [edi + 0x14], eax
dormant_loc_43c39f:
        add	edi, 0x18
        mov	dword ptr [pRLEOutputCursor], edi
        mov	eax, dword ptr [nRLEEncodedMaximumX]
        inc	eax
        sub	eax, dword ptr [nRLEEncodedMinimumX]
        mov	dword ptr [ebp - 0x18], eax
        mov	eax, dword ptr [nRLEEncodedMinimumY]
        imul	dword ptr [ebp - 0x30]
        add	eax, dword ptr [ebp - 0x2c]
        add	eax, dword ptr [nRLEEncodedMinimumX]
        mov	esi, eax
        mov	dword ptr [pRLEScanlineStart], esi
        mov	eax, dword ptr [nRLEEncodedMinimumY]
        mov	dword ptr [ebp - 0x10], eax
        jmp dormant_loc_43c3f7
dormant_loc_43c3da:
        push	dword ptr [ebp - 0x1c]
        push	dword ptr [ebp + 0xc]
        push	dword ptr [ebp - 0x18]
        call EncodeRLEScanline
        add	esp, 0xc
        mov	eax, dword ptr [ebp - 0x30]
        add	dword ptr [pRLEScanlineStart], eax
        inc	dword ptr [ebp - 0x10]
dormant_loc_43c3f7:
        mov	eax, dword ptr [ebp - 0x10]
        cmp	eax, dword ptr [nRLEEncodedMaximumY]
        jle dormant_loc_43c3da
        mov	eax, dword ptr [pRLEOutputCursor]
        sub	eax, dword ptr [ebp + 0x18]
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
