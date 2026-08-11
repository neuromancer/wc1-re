    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0x18
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov	edi, dword ptr [ebp + 0x8]
        mov	dword ptr [g_pGifRasterTarget_0046ff28], edi
        mov	edi, dword ptr [ebp + 0x10]
        mov	eax, 0x0
        mov	ecx, 0x2e
        mov	edx, ecx
        and	ecx, 0x3
        rep		stosb
        mov	ecx, edx
        shr	ecx, 0x2
        rep		stosd
        mov	edi, dword ptr [ebp + 0x10]
        mov	esi, dword ptr [ebp + 0xc]
        movzx	eax, byte ptr [esi + 0xb]
        mov	dword ptr [ebp - 0x18], eax
        mov	al, byte ptr [esi + 0xa]
        mov	cl, al
        and	cl, 0x7
        inc	cl
        mov	ebx, 0x1
        shl	ebx, cl
        add	esi, 0xd
        test	al, -0x80
        je dormant_loc_43ec84
        imul	ebx, ebx, 0x3
        add	esi, ebx
dormant_loc_43ec84:
        movzx	eax, word ptr [esi + 0x5]
        mov	dword ptr [edi + 0x24], eax
        movzx	eax, word ptr [esi + 0x7]
        mov	dword ptr [edi + 0x28], eax
        movzx	eax, byte ptr [esi + 0x9]
        mov	byte ptr [edi + 0x2c], al
        and	byte ptr [edi + 0x2c], 0x40
        add	esi, 0xa
        test	al, -0x80
        je dormant_loc_43ecb7
        mov	cl, al
        and	cl, 0x7
        inc	cl
        mov	ebx, 0x1
        shl	ebx, cl
        imul	ebx, ebx, 0x3
        add	esi, ebx
dormant_loc_43ecb7:
        mov	dword ptr [edi + 0x10], 0x0
        lodsb
        movzx	ecx, al
        mov	edx, 0x8
        push	ecx
        push	edx
        mov	eax, 0x1
        shl	eax, cl
        mov	dword ptr [ebp - 0x4], eax
        inc	eax
        mov	dword ptr [ebp - 0x8], eax
        inc	ecx
        mov	dword ptr [edi + 0x1c], ecx
        mov	ecx, dword ptr [ebp - 0x4]
        call ResetGIFLZWDictionary
        mov	dword ptr [ebp - 0x10], 0xffff
        mov	dword ptr [ebp - 0x14], 0x0
        mov	byte ptr [edi + 0x2d], 0x0
        mov	eax, dword ptr [edi + 0x24]
        mov	dword ptr [edi + 0x20], eax
        mov	dword ptr [edi + 0x8], 0x0
        mov	dword ptr [edi + 0xc], 0x0
        pop	edx
        pop	ecx
dormant_loc_43ed0b:
        push	ecx
        push	edx
        mov	edx, dword ptr [edi + 0x1c]
        cmp	edx, 0x8
        jg dormant_loc_43ed1e
        push	edx
        call ReadGIFLZWCode
        pop	edx
        jmp dormant_loc_43ed3b
dormant_loc_43ed1e:
        push	edx
        mov	edx, 0x8
        call ReadGIFLZWCode
        pop	edx
        push	eax
        push	edx
        sub	edx, 0x8
        call ReadGIFLZWCode
        pop	edx
        shl	eax, 0x8
        pop	ebx
        or	eax, ebx
dormant_loc_43ed3b:
        mov	dword ptr [ebp - 0xc], eax
        pop	edx
        pop	ecx
        cmp	eax, dword ptr [ebp - 0x4]
        jne dormant_loc_43ed63
        push	ecx
        push	edx
        mov	ecx, dword ptr [ebp - 0x4]
        call ResetGIFLZWDictionary
        pop	edx
        pop	ecx
        mov	eax, ecx
        inc	eax
        mov	dword ptr [edi + 0x1c], eax
        mov	dword ptr [ebp - 0x10], 0xffff
        jmp dormant_loc_43ee2e
dormant_loc_43ed63:
        cmp	eax, dword ptr [ebp - 0x8]
        jne dormant_loc_43ed8f
dormant_loc_43ed68:
        cmp	dword ptr [edi + 0x10], 0x0
        je dormant_loc_43ed74
        lodsb
        dec	dword ptr [edi + 0x10]
        jmp dormant_loc_43ed68
dormant_loc_43ed74:
        lodsb
        and	eax, 0xff
        mov	dword ptr [edi + 0x10], eax
        cmp	dword ptr [edi + 0x10], 0x0
        jne dormant_loc_43ed68
        mov	dword ptr [ebp - 0x14], 0xffff
        jmp dormant_loc_43ee2e
dormant_loc_43ed8f:
        mov	ebx, dword ptr [ebp - 0xc]
        cmp	word ptr [edi + 2*ebx + 0x302e], -0x2
        je dormant_loc_43edb4
        cmp	dword ptr [ebp - 0x10], 0xffff
        je dormant_loc_43edb2
        push	ecx
        push	edx
        mov	ecx, dword ptr [ebp - 0x10]
        call AppendGIFLZWDictionaryEntry
        pop	edx
        pop	ecx
dormant_loc_43edb2:
        jmp dormant_loc_43edc3
dormant_loc_43edb4:
        push	ecx
        push	edx
        mov	ebx, dword ptr [ebp - 0x10]
        mov	ecx, dword ptr [ebp - 0x10]
        call AppendGIFLZWDictionaryEntry
        pop	edx
        pop	ecx
dormant_loc_43edc3:
        push	ecx
        push	edx
        mov	ebx, dword ptr [ebp - 0xc]
        push	esi
        mov	ecx, 0x0
        mov	esi, edi
        add	esi, 0x2e
dormant_loc_43edd3:
        mov	al, byte ptr [ebx + edi + 0x202e]
        mov	byte ptr [esi], al
        inc	esi
        inc	ecx
        movzx	ebx, word ptr [edi + 2*ebx + 0x302e]
        cmp	ebx, 0xffff
        jne dormant_loc_43edd3
        cmp	edx, 0x1
        jne dormant_loc_43ee14
dormant_loc_43edf3:
        dec	esi
        mov	al, byte ptr [esi]
        and	eax, 0x1
        push	ecx
        call EmitGIFDecodedPixel
        pop	ecx
        mov	al, byte ptr [esi]
        and	eax, 0xff
        shr	eax, 1
        push	ecx
        call EmitGIFDecodedPixel
        pop	ecx
        loop dormant_loc_43edf3
        jmp dormant_loc_43ee25
dormant_loc_43ee14:
        dec	esi
        mov	al, byte ptr [esi]
        and	eax, 0xff
        push	ecx
        call EmitGIFDecodedPixel
        pop	ecx
        loop dormant_loc_43ee14
dormant_loc_43ee25:
        pop	esi
        pop	edx
        pop	ecx
        mov	eax, dword ptr [ebp - 0xc]
        mov	dword ptr [ebp - 0x10], eax
dormant_loc_43ee2e:
        cmp	dword ptr [ebp - 0x14], 0x0
        jne dormant_loc_43ee39
        jmp dormant_loc_43ed0b
dormant_loc_43ee39:
        mov	eax, dword ptr [ebp - 0x18]
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
