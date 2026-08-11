    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0x4
        push	ebx
        push	esi
        push	edi
        push	es
        mov	esi, dword ptr [ebp + 0x8]
        mov	ecx, dword ptr [esi + 0x4]
        dec	ecx
        add	esi, 0x8
        mov	dword ptr [ebp - 0x4], esi
        mov	ebx, 0x1
        mov	edx, dword ptr [ebp + 0xc]
        cmp	edx, 0x0
        je dormant_loc_43f09b
        mov	dword ptr [edx], 0x0
        add	edx, 0x4
dormant_loc_43f09b:
        cmp	ecx, 0x0
        je dormant_loc_43f0c8
dormant_loc_43f0a0:
        add	esi, 0x8
        mov	eax, dword ptr [esi]
        mov	edi, dword ptr [ebp - 0x4]
dormant_loc_43f0a8:
        cmp	eax, dword ptr [edi]
        je dormant_loc_43f0c6
        add	edi, 0x8
        cmp	edi, esi
        jl dormant_loc_43f0a8
        cmp	edx, 0x0
        je dormant_loc_43f0c5
        mov	eax, esi
        sub	eax, dword ptr [ebp - 0x4]
        shr	eax, 0x3
        mov	dword ptr [edx], eax
        add	edx, 0x4
dormant_loc_43f0c5:
        inc	ebx
dormant_loc_43f0c6:
        loop dormant_loc_43f0a0
dormant_loc_43f0c8:
        mov	eax, ebx
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
