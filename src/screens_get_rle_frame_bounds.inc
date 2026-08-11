    __asm {
        push	ebp
        mov	ebp, esp
        add	esp, -0x24
        push	ebx
        push	esi
        push	edi
        push	es
        cld
        push	ds
        pop	es
        mov	dword ptr [ebp - 0x18], 0x0
        mov	dword ptr [ebp - 0x1c], 0x0
        mov	dword ptr [ebp - 0x20], 0x0
        mov	dword ptr [ebp - 0x24], 0x0
        mov	esi, dword ptr [ebp + 0xc]
        shl	esi, 0x3
        add	esi, 0x8
        add	esi, dword ptr [ebp + 0x8]
        mov	esi, dword ptr [esi]
        add	esi, dword ptr [ebp + 0x8]
        mov	dword ptr [ebp - 0x4], esi
        mov	esi, dword ptr [ebp - 0x4]
        mov	eax, dword ptr [esi + 0x8]
        add	eax, dword ptr [ebp + 0x10]
        mov	dword ptr [ebp - 0x8], eax
        mov	eax, dword ptr [esi + 0xc]
        mov	ebx, eax
        add	eax, dword ptr [ebp + 0x14]
        mov	dword ptr [ebp - 0xc], eax
        mov	eax, dword ptr [esi + 0x10]
        add	eax, dword ptr [ebp + 0x10]
        mov	dword ptr [ebp - 0x10], eax
        mov	eax, dword ptr [esi + 0x14]
        mov	ecx, eax
        add	ecx, dword ptr [ebp + 0x14]
        mov	dword ptr [ebp - 0x14], ecx
        inc	eax
        sub	eax, ebx
        mov	ebx, eax
        jle dormant_loc_43c13a
        add	esi, 0x18
        mov	dword ptr [ebp - 0x18], 0x7fffffff
        mov	dword ptr [ebp - 0x1c], 0x7fffffff
        mov	dword ptr [ebp - 0x20], 0x80000000
        mov	dword ptr [ebp - 0x24], 0x80000000
        mov	edx, dword ptr [ebp - 0xc]
dormant_loc_43c0aa:
        mov	edi, dword ptr [ebp - 0x8]
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c0cd
        jne dormant_loc_43c100
        jae dormant_loc_43c132
dormant_loc_43c0b8:
        mov	al, byte ptr [esi]
        inc	esi
        movzx	ecx, al
        add	edi, ecx
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c0cd
        jne dormant_loc_43c100
        jb dormant_loc_43c0b8
        jae dormant_loc_43c132
dormant_loc_43c0cd:
        movzx	ecx, al
        mov	al, byte ptr [esi]
        inc	esi
        cmp	dword ptr [ebp - 0x18], edi
        jl dormant_loc_43c0db
        mov	dword ptr [ebp - 0x18], edi
dormant_loc_43c0db:
        add	edi, ecx
        cmp	dword ptr [ebp - 0x20], edi
        jg dormant_loc_43c0e5
        mov	dword ptr [ebp - 0x20], edi
dormant_loc_43c0e5:
        cmp	dword ptr [ebp - 0x1c], edx
        jl dormant_loc_43c0ed
        mov	dword ptr [ebp - 0x1c], edx
dormant_loc_43c0ed:
        cmp	dword ptr [ebp - 0x24], edx
        jg dormant_loc_43c0f5
        mov	dword ptr [ebp - 0x24], edx
dormant_loc_43c0f5:
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c0cd
        jae dormant_loc_43c132
        je dormant_loc_43c0b8
dormant_loc_43c100:
        movzx	ecx, al
        add	esi, ecx
        cmp	dword ptr [ebp - 0x18], edi
        jl dormant_loc_43c10d
        mov	dword ptr [ebp - 0x18], edi
dormant_loc_43c10d:
        add	edi, ecx
        cmp	dword ptr [ebp - 0x20], edi
        jg dormant_loc_43c117
        mov	dword ptr [ebp - 0x20], edi
dormant_loc_43c117:
        cmp	dword ptr [ebp - 0x1c], edx
        jl dormant_loc_43c11f
        mov	dword ptr [ebp - 0x1c], edx
dormant_loc_43c11f:
        cmp	dword ptr [ebp - 0x24], edx
        jg dormant_loc_43c127
        mov	dword ptr [ebp - 0x24], edx
dormant_loc_43c127:
        mov	al, byte ptr [esi]
        inc	esi
        shr	al, 1
        ja dormant_loc_43c0cd
        jne dormant_loc_43c100
        jb dormant_loc_43c0b8
dormant_loc_43c132:
        inc	edx
        dec	ebx
        jne dormant_loc_43c0aa
dormant_loc_43c13a:
        mov	eax, dword ptr [ebp + 0x18]
        and	eax, 0x1
        je dormant_loc_43c156
        mov	eax, dword ptr [ebp + 0x10]
        add	eax, dword ptr [ebp + 0x10]
        mov	ebx, eax
        sub	eax, dword ptr [ebp - 0x18]
        sub	ebx, dword ptr [ebp - 0x20]
        mov	dword ptr [ebp - 0x20], eax
        mov	dword ptr [ebp - 0x18], ebx
dormant_loc_43c156:
        mov	eax, dword ptr [ebp + 0x18]
        and	eax, 0x2
        je dormant_loc_43c172
        mov	eax, dword ptr [ebp + 0x14]
        add	eax, dword ptr [ebp + 0x14]
        mov	ebx, eax
        sub	eax, dword ptr [ebp - 0x1c]
        sub	ebx, dword ptr [ebp - 0x24]
        mov	dword ptr [ebp - 0x24], eax
        mov	dword ptr [ebp - 0x1c], ebx
dormant_loc_43c172:
        mov	edi, dword ptr [ebp + 0x1c]
        mov	eax, dword ptr [ebp - 0x18]
        stosd
        mov	eax, dword ptr [ebp - 0x1c]
        stosd
        mov	eax, dword ptr [ebp - 0x20]
        stosd
        mov	eax, dword ptr [ebp - 0x24]
        stosd
        xor	eax, eax
        pop	es
        pop	edi
        pop	esi
        pop	ebx
        _emit 0xc9
        ret
    }
