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
        mov	esi, dword ptr [ebp + 0xc]
        shl	esi, 0x3
        add	esi, 0x8
        add	esi, dword ptr [ebp + 0x8]
        mov	esi, dword ptr [esi]
        add	esi, dword ptr [ebp + 0x8]
        mov	ebx, dword ptr [esi + 0xc]
        mov	eax, dword ptr [esi + 0x14]
        inc	eax
        sub	eax, ebx
        mov	ebx, eax
        jle dormant_loc_43c49a
        add	esi, 0x18
dormant_loc_43c43b:
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c459
        jne dormant_loc_43c477
        jae dormant_loc_43c497
dormant_loc_43c446:
        mov	al, byte ptr [esi]
        inc	esi
        movzx	ecx, al
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c459
        jne dormant_loc_43c477
        jb dormant_loc_43c446
        jae dormant_loc_43c497
dormant_loc_43c459:
        movzx	ecx, al
        mov	eax, 0x0
        mov	al, byte ptr [esi]
        mov al, byte ptr abRasterPaletteTranslation[eax]
        mov	byte ptr [esi], al
        inc	esi
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c459
        jae dormant_loc_43c497
        je dormant_loc_43c446
dormant_loc_43c477:
        movzx	ecx, al
        mov	eax, 0x0
dormant_loc_43c47f:
        mov	al, byte ptr [esi]
        mov al, byte ptr abRasterPaletteTranslation[eax]
        mov	byte ptr [esi], al
        inc	esi
        loop dormant_loc_43c47f
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c459
        jne dormant_loc_43c477
        jb dormant_loc_43c446
dormant_loc_43c497:
        dec	ebx
        jne dormant_loc_43c43b
dormant_loc_43c49a:
        xor	eax, eax
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
