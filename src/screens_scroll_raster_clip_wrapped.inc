    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0x44
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov	esi, dword ptr [ebp + 0x8]
        mov	eax, dword ptr [esi + 0xc]
        inc	eax
        sub	eax, dword ptr [esi + 0x4]
        mov	dword ptr [ebp - 0x30], eax
        jle dormant_loc_43ce75
        mov	edx, dword ptr [esi + 0x10]
        inc	edx
        sub	edx, dword ptr [esi + 0x8]
        mov	dword ptr [ebp - 0x34], edx
        jle dormant_loc_43ce75
        neg	eax
        mov	dword ptr [ebp - 0x38], eax
        neg	edx
        mov	dword ptr [ebp - 0x3c], edx
        cmp	dword ptr [ebp + 0x14], 0x1
        je dormant_loc_43cd0a
        mov	eax, dword ptr [ebp + 0xc]
        cdq
        xor	eax, edx
        sub	eax, edx
        cmp	eax, dword ptr [ebp - 0x30]
        jge dormant_loc_43ccee
        mov	eax, dword ptr [ebp + 0x10]
        cdq
        xor	eax, edx
        sub	eax, edx
        cmp	eax, dword ptr [ebp - 0x34]
        jge dormant_loc_43ccee
        mov	eax, dword ptr [ebp + 0x8]
        mov	dword ptr [ebp - 0x2c], eax
        mov	eax, dword ptr [ebp + 0x18]
        mov	dword ptr [ebp - 0x44], eax
        jmp dormant_loc_43cd75
dormant_loc_43ccee:
        mov	eax, dword ptr [ebp + 0x18]
        push	0x0
        movzx	ax, al
        push	ax
        push	dword ptr [ebp + 0x8]
        call FillRasterClip
        add	esp, 0x8
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
dormant_loc_43cd0a:
        cmp	dword ptr [ebp + 0x18], 0x0
        je dormant_loc_43ce69
        lea	eax, [ebp - 0x28]
        mov	dword ptr [ebp - 0x2c], eax
        mov	eax, dword ptr [ebp + 0x18]
        mov	dword ptr [ebp - 0x14], eax
        lea	eax, [ebp - 0x14]
        mov	dword ptr [ebp - 0x28], eax
        xor	eax, eax
        mov	dword ptr [ebp - 0x24], eax
        mov	dword ptr [ebp - 0x20], eax
        mov	eax, dword ptr [ebp - 0x30]
        dec	eax
        mov	dword ptr [ebp - 0x10], eax
        mov	dword ptr [ebp - 0x1c], eax
        mov	eax, dword ptr [ebp - 0x34]
        dec	eax
        mov	dword ptr [ebp - 0xc], eax
        mov	dword ptr [ebp - 0x18], eax
        push	-0x1
        push	0x0
        push	0x0
        push	dword ptr [ebp - 0x2c]
        push	0x0
        push	0x0
        push	dword ptr [ebp + 0x8]
        call BlitRasterClip
        add	esp, 0x1c
        mov	eax, dword ptr [ebp + 0xc]
        cdq
        idiv	dword ptr [ebp - 0x30]
        mov	dword ptr [ebp + 0xc], edx
        mov	eax, dword ptr [ebp + 0x10]
        cdq
        idiv	dword ptr [ebp - 0x34]
        mov	dword ptr [ebp + 0x10], edx
        mov	dword ptr [ebp - 0x44], 0xffffffff
dormant_loc_43cd75:
        mov	eax, dword ptr [ebp + 0xc]
        or	eax, dword ptr [ebp + 0x10]
        je dormant_loc_43ce61
        mov	esi, dword ptr [ebp - 0x2c]
        mov	edi, dword ptr [ebp + 0x8]
        push	-0x1
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	0x0
        push	0x0
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	dword ptr [ebp - 0x34]
        push	dword ptr [ebp - 0x30]
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	0x0
        push	dword ptr [ebp - 0x30]
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	dword ptr [ebp - 0x3c]
        push	dword ptr [ebp - 0x30]
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	dword ptr [ebp - 0x34]
        push	0x0
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	dword ptr [ebp - 0x3c]
        push	0x0
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	dword ptr [ebp - 0x34]
        push	dword ptr [ebp - 0x38]
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	0x0
        push	dword ptr [ebp - 0x38]
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
        push	dword ptr [ebp - 0x44]
        push	dword ptr [ebp + 0x10]
        push	dword ptr [ebp + 0xc]
        push	edi
        push	dword ptr [ebp - 0x3c]
        push	dword ptr [ebp - 0x38]
        push	esi
        call BlitRasterClip
        add	esp, 0x1c
dormant_loc_43ce61:
        xor	eax, eax
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
dormant_loc_43ce69:
        mov	eax, dword ptr [ebp - 0x30]
        mul	dword ptr [ebp - 0x34]
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
dormant_loc_43ce75:
        mov	eax, 0xfffffffe
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
