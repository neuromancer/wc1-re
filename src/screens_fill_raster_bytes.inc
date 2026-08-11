    __asm {
        mov	edi, edi
        mov	edx, dword ptr [esp + 0x8]
        push	edi
        movsx	eax, word ptr [esp + 0x10]
        mov	cl, al
        mov	edi, dword ptr [esp + 0x8]
        mov	ch, cl
        mov	cl, al
        mov	eax, ecx
        shl	eax, 0x10
        mov	ax, cx
        mov	ecx, edx
        shr	ecx, 0x2
        rep		stosd
        mov	ecx, edx
        and	ecx, 0x3
        rep		stosb
        pop	edi
        ret	0xc
    }
