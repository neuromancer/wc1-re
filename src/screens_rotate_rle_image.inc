    __asm {
        push ebp
        mov ebp, esp
        add esp, 0xffffff10
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        cmp dword ptr [ebp + 0x24], 0x10000
        jne rotate_rle_43b495
        cmp dword ptr [ebp + 0x28], 0x10000
        jne rotate_rle_43b495
        cmp dword ptr [ebp + 0x20], 0x0
        je rotate_rle_43bfd3
rotate_rle_43b495:
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        call GetRLEImageSize
        add esp, 0x8
        mov ecx, eax
        shr eax, 0x10
        dec eax
        mov dword ptr [ebp - 0x44], eax
        and ecx, 0xffff
        dec ecx
        mov dword ptr [ebp - 0x48], ecx
        lea ebx, [ebp - 0x14]
        lea esi, [ebp - 0x28]
        mov dword ptr [esi], ebx
        mov edx, dword ptr [ebp + 0x1c]
        mov dword ptr [ebx], edx
        mov dword ptr [esi + 0x4], 0x0
        mov dword ptr [aRLETransformVertices + 0xc], 0x0
        mov dword ptr [aRLETransformVertices + 0x48], 0x0
        mov dword ptr [esi + 0x8], 0x0
        mov dword ptr [aRLETransformVertices + 0x10], 0x0
        mov dword ptr [aRLETransformVertices + 0x24], 0x0
        mov dword ptr [ebx + 0x4], eax
        mov dword ptr [esi + 0xc], eax
        mov dword ptr [aRLETransformVertices + 0x20], eax
        mov dword ptr [aRLETransformVertices + 0x34], eax
        mov dword ptr [ebx + 0x8], ecx
        mov dword ptr [esi + 0x10], ecx
        mov dword ptr [aRLETransformVertices + 0x38], ecx
        mov dword ptr [aRLETransformVertices + 0x4c], ecx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        call GetRLEImageOrigin
        add esp, 0x8
        mov ebx, eax
        cwde
        neg eax
        mov dword ptr [ebp - 0x3c], eax
        sar ebx, 0x10
        neg ebx
        mov dword ptr [ebp - 0x40], ebx
        test dword ptr [ebp + 0x2c], 0x2
        jne rotate_rle_43b58e
        push 0xff
        lea eax, [ebp - 0x28]
        push eax
        call FillRasterClip
        add esp, 0x8
        test dword ptr [ebp + 0x2c], 0x1
        je rotate_rle_43b576
        push dword ptr [ebp - 0x3c]
        push dword ptr [ebp - 0x40]
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        lea eax, [ebp - 0x28]
        push eax
        call DrawRLEImageColor
        add esp, 0x14
        jmp rotate_rle_43b58e
rotate_rle_43b576:
        push dword ptr [ebp - 0x3c]
        push dword ptr [ebp - 0x40]
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        lea eax, [ebp - 0x28]
        push eax
        call DrawRLEImage
        add esp, 0x14
rotate_rle_43b58e:
        mov esi, dword ptr [ebp + 0x8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 0x4]
        inc eax
        mov dword ptr [ebp - 0xe8], eax
        jle rotate_rle_43b62f
        mov eax, dword ptr [ebx + 0x8]
        inc eax
        mov ecx, eax
        jle rotate_rle_43b62f
        mov eax, dword ptr [esi + 0x4]
        mov dword ptr [ebp - 0xec], eax
        cmp eax, 0x0
        jg rotate_rle_43b5c2
        mov eax, 0x0
rotate_rle_43b5c2:
        mov dword ptr [ebp - 0xd4], eax
        mov eax, dword ptr [esi + 0x8]
        mov dword ptr [ebp - 0xf0], eax
        cmp eax, 0x0
        jg rotate_rle_43b5db
        mov eax, 0x0
rotate_rle_43b5db:
        mov dword ptr [ebp - 0xd8], eax
        mov eax, dword ptr [esi + 0xc]
        mov edx, dword ptr [ebp - 0xe8]
        dec edx
        cmp eax, edx
        jl rotate_rle_43b5f1
        mov eax, edx
rotate_rle_43b5f1:
        mov dword ptr [ebp - 0xdc], eax
        mov eax, dword ptr [esi + 0x10]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl rotate_rle_43b603
        mov eax, edx
rotate_rle_43b603:
        mov dword ptr [ebp - 0xe0], eax
        mov eax, dword ptr [ebp - 0xdc]
        cmp eax, dword ptr [ebp - 0xd4]
        jl rotate_rle_43b63a
        mov eax, dword ptr [ebp - 0xe0]
        cmp eax, dword ptr [ebp - 0xd8]
        jl rotate_rle_43b63a
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 0xe4], eax
        jmp rotate_rle_43b645
rotate_rle_43b62f:
        mov eax, 0xffffffff
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
rotate_rle_43b63a:
        mov eax, 0xfffffffe
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
rotate_rle_43b645:
        mov eax, dword ptr [ebp + 0x14]
        sub eax, dword ptr [ebp - 0x40]
        mov dword ptr [ebp - 0x4c], eax
        mov eax, dword ptr [ebp + 0x18]
        sub eax, dword ptr [ebp - 0x3c]
        mov dword ptr [ebp - 0x50], eax
        mov dword ptr [ebp - 0x30], 0x0
        mov dword ptr [ebp - 0x2c], 0x0
        push dword ptr [ebp + 0x28]
        push dword ptr [ebp + 0x24]
        push dword ptr [ebp + 0x20]
        lea eax, [ebp - 0x40]
        push eax
        lea eax, [ebp - 0x38]
        push eax
        lea eax, [ebp - 0x30]
        push eax
        call TransformRLEPoint
        add esp, 0x18
        mov eax, dword ptr [ebp - 0x38]
        add eax, dword ptr [ebp - 0x4c]
        mov dword ptr [aRLETransformVertices], eax
        mov eax, dword ptr [ebp - 0x34]
        add eax, dword ptr [ebp - 0x50]
        mov dword ptr [aRLETransformVertices + 0x4], eax
        mov eax, dword ptr [ebp - 0x44]
        mov dword ptr [ebp - 0x30], eax
        mov dword ptr [ebp - 0x2c], 0x0
        push dword ptr [ebp + 0x28]
        push dword ptr [ebp + 0x24]
        push dword ptr [ebp + 0x20]
        lea eax, [ebp - 0x40]
        push eax
        lea eax, [ebp - 0x38]
        push eax
        lea eax, [ebp - 0x30]
        push eax
        call TransformRLEPoint
        add esp, 0x18
        mov eax, dword ptr [ebp - 0x38]
        add eax, dword ptr [ebp - 0x4c]
        mov dword ptr [aRLETransformVertices + 0x14], eax
        mov eax, dword ptr [ebp - 0x34]
        add eax, dword ptr [ebp - 0x50]
        mov dword ptr [aRLETransformVertices + 0x18], eax
        mov eax, dword ptr [ebp - 0xec]
        add dword ptr [aRLETransformVertices], eax
        add dword ptr [aRLETransformVertices + 0x14], eax
        mov eax, dword ptr [ebp - 0xf0]
        add dword ptr [aRLETransformVertices + 0x4], eax
        add dword ptr [aRLETransformVertices + 0x18], eax
        mov eax, dword ptr [ebp - 0x44]
        mov ebx, dword ptr [ebp - 0x48]
        mov dword ptr [ebp - 0x30], eax
        mov dword ptr [ebp - 0x2c], ebx
        push dword ptr [ebp + 0x28]
        push dword ptr [ebp + 0x24]
        push dword ptr [ebp + 0x20]
        lea eax, [ebp - 0x40]
        push eax
        lea eax, [ebp - 0x38]
        push eax
        lea eax, [ebp - 0x30]
        push eax
        call TransformRLEPoint
        add esp, 0x18
        mov eax, dword ptr [ebp - 0x38]
        add eax, dword ptr [ebp - 0x4c]
        mov dword ptr [aRLETransformVertices + 0x28], eax
        mov eax, dword ptr [ebp - 0x34]
        add eax, dword ptr [ebp - 0x50]
        mov dword ptr [aRLETransformVertices + 0x2c], eax
        mov eax, dword ptr [ebp - 0x48]
        mov dword ptr [ebp - 0x30], 0x0
        mov dword ptr [ebp - 0x2c], eax
        push dword ptr [ebp + 0x28]
        push dword ptr [ebp + 0x24]
        push dword ptr [ebp + 0x20]
        lea eax, [ebp - 0x40]
        push eax
        lea eax, [ebp - 0x38]
        push eax
        lea eax, [ebp - 0x30]
        push eax
        call TransformRLEPoint
        add esp, 0x18
        mov eax, dword ptr [ebp - 0x38]
        add eax, dword ptr [ebp - 0x4c]
        mov dword ptr [aRLETransformVertices + 0x3c], eax
        mov eax, dword ptr [ebp - 0x34]
        add eax, dword ptr [ebp - 0x50]
        mov dword ptr [aRLETransformVertices + 0x40], eax
        mov eax, dword ptr [ebp - 0xec]
        add dword ptr [aRLETransformVertices + 0x28], eax
        add dword ptr [aRLETransformVertices + 0x3c], eax
        mov eax, dword ptr [ebp - 0xf0]
        add dword ptr [aRLETransformVertices + 0x2c], eax
        add dword ptr [aRLETransformVertices + 0x40], eax
        lea ebx, [ebp - 0x14]
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 0x58], eax
        mov ecx, dword ptr [ebx + 0x4]
        inc ecx
        mov dword ptr [ebp - 0x54], ecx
        push ds
        pop es
        mov ebx, offset aRLETransformVertices
        mov eax, ebx
        add eax, 0x50
        mov dword ptr [ebp - 0x64], ebx
        mov dword ptr [ebp - 0x68], eax
        mov esi, 0x7fff
        mov edi, 0xffff8000
        mov ecx, 0xf
rotate_rle_43b7cf:
        mov edx, 0x0
        mov eax, dword ptr [ebx]
        sub eax, dword ptr [ebp - 0xd4]
        shld edx, eax, 0x1
        mov eax, dword ptr [ebp - 0xdc]
        sub eax, dword ptr [ebx]
        shld edx, eax, 0x1
        mov eax, dword ptr [ebx + 0x4]
        sub eax, dword ptr [ebp - 0xd8]
        shld edx, eax, 0x1
        mov eax, dword ptr [ebp - 0xe0]
        sub eax, dword ptr [ebx + 0x4]
        shld edx, eax, 0x1
        mov eax, dword ptr [ebx + 0x4]
        cmp eax, esi
        jg rotate_rle_43b812
        mov esi, eax
        mov dword ptr [ebp - 0x6c], ebx
rotate_rle_43b812:
        cmp eax, edi
        jl rotate_rle_43b818
        mov edi, eax
rotate_rle_43b818:
        and ecx, edx
        add ebx, 0x14
        cmp ebx, dword ptr [ebp - 0x68]
        jne rotate_rle_43b7cf
        or ecx, ecx
        jne rotate_rle_43be04
        mov eax, dword ptr [ebp - 0x6c]
        mov dword ptr [ebp - 0x78], eax
        mov dword ptr [ebp - 0x7c], eax
        mov dword ptr [ebp - 0x8c], esi
        cmp edi, esi
        je rotate_rle_43be04
rotate_rle_43b841:
        mov ebx, dword ptr [ebp - 0x78]
        mov dword ptr [ebp - 0x70], ebx
        mov esi, ebx
        sub esi, 0x14
        cmp esi, dword ptr [ebp - 0x64]
        jge rotate_rle_43b857
        mov esi, dword ptr [ebp - 0x68]
        sub esi, 0x14
rotate_rle_43b857:
        mov dword ptr [ebp - 0x78], esi
        mov ecx, dword ptr [esi + 0x4]
        mov edx, dword ptr [ebx + 0x4]
        cmp edx, dword ptr [ebp - 0xd8]
        jge rotate_rle_43b870
        cmp ecx, dword ptr [ebp - 0xd8]
        jle rotate_rle_43b841
rotate_rle_43b870:
        sub ecx, edx
        je rotate_rle_43b841
        mov dword ptr [ebp - 0x80], ecx
        mov edx, dword ptr [esi]
        sub edx, dword ptr [ebx]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xa8], eax
        mov ecx, dword ptr [ebp - 0x80]
        mov edx, dword ptr [esi + 0xc]
        sub edx, dword ptr [ebx + 0xc]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xb0], eax
        mov ecx, dword ptr [ebp - 0x80]
        mov edx, dword ptr [esi + 0x10]
        sub edx, dword ptr [ebx + 0x10]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xb8], eax
        mov edx, dword ptr [ebx]
        shl edx, 0x10
        add edx, 0x8000
        mov dword ptr [ebp - 0x90], edx
        mov edx, dword ptr [ebx + 0xc]
        shl edx, 0x10
        add edx, 0x8000
        mov dword ptr [ebp - 0x98], edx
        mov edx, dword ptr [ebx + 0x10]
        shl edx, 0x10
        add edx, 0x8000
        mov dword ptr [ebp - 0xa0], edx
rotate_rle_43b907:
        mov ebx, dword ptr [ebp - 0x7c]
        mov dword ptr [ebp - 0x74], ebx
        mov esi, ebx
        add esi, 0x14
        cmp esi, dword ptr [ebp - 0x68]
        jl rotate_rle_43b91a
        mov esi, dword ptr [ebp - 0x64]
rotate_rle_43b91a:
        mov dword ptr [ebp - 0x7c], esi
        mov ecx, dword ptr [esi + 0x4]
        mov edx, dword ptr [ebx + 0x4]
        cmp edx, dword ptr [ebp - 0xd8]
        jge rotate_rle_43b933
        cmp ecx, dword ptr [ebp - 0xd8]
        jle rotate_rle_43b907
rotate_rle_43b933:
        sub ecx, edx
        je rotate_rle_43b907
        mov dword ptr [ebp - 0x84], ecx
        mov edx, dword ptr [esi]
        sub edx, dword ptr [ebx]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xac], eax
        mov ecx, dword ptr [ebp - 0x84]
        mov edx, dword ptr [esi + 0xc]
        sub edx, dword ptr [ebx + 0xc]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xb4], eax
        mov ecx, dword ptr [ebp - 0x84]
        mov edx, dword ptr [esi + 0x10]
        sub edx, dword ptr [ebx + 0x10]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xbc], eax
        mov edx, dword ptr [ebx]
        shl edx, 0x10
        add edx, 0x8000
        mov dword ptr [ebp - 0x94], edx
        mov edx, dword ptr [ebx + 0xc]
        shl edx, 0x10
        add edx, 0x8000
        mov dword ptr [ebp - 0x9c], edx
        mov edx, dword ptr [ebx + 0x10]
        shl edx, 0x10
        add edx, 0x8000
        mov dword ptr [ebp - 0xa4], edx
        mov eax, dword ptr [ebp - 0xe0]
        sub eax, dword ptr [ebp - 0x8c]
        sub edi, dword ptr [ebp - 0xe0]
        jg rotate_rle_43b9e9
        add eax, edi
rotate_rle_43b9e9:
        mov dword ptr [ebp - 0x88], eax
        mov eax, dword ptr [ebp - 0xd8]
        sub eax, dword ptr [ebp - 0x8c]
        jle rotate_rle_43baa0
        sub dword ptr [ebp - 0x88], eax
        mov ecx, dword ptr [ebp - 0xd8]
        mov dword ptr [ebp - 0x8c], ecx
        mov ebx, dword ptr [ebp - 0x70]
        sub ecx, dword ptr [ebx + 0x4]
        sub dword ptr [ebp - 0x80], ecx
        shl ecx, 0x10
        mov eax, dword ptr [ebp - 0xa8]
        imul ecx
        shrd eax, edx, 0x10
        add dword ptr [ebp - 0x90], eax
        mov eax, dword ptr [ebp - 0xb0]
        imul ecx
        shrd eax, edx, 0x10
        add dword ptr [ebp - 0x98], eax
        mov eax, dword ptr [ebp - 0xb8]
        imul ecx
        shrd eax, edx, 0x10
        add dword ptr [ebp - 0xa0], eax
        mov ecx, dword ptr [ebp - 0xd8]
        mov ebx, dword ptr [ebp - 0x74]
        sub ecx, dword ptr [ebx + 0x4]
        sub dword ptr [ebp - 0x84], ecx
        shl ecx, 0x10
        mov eax, dword ptr [ebp - 0xac]
        imul ecx
        shrd eax, edx, 0x10
        add dword ptr [ebp - 0x94], eax
        mov eax, dword ptr [ebp - 0xb4]
        imul ecx
        shrd eax, edx, 0x10
        add dword ptr [ebp - 0x9c], eax
        mov eax, dword ptr [ebp - 0xbc]
        imul ecx
        shrd eax, edx, 0x10
        add dword ptr [ebp - 0xa4], eax
rotate_rle_43baa0:
        mov eax, dword ptr [ebp - 0x8c]
        imul dword ptr [ebp - 0xe8]
        add eax, dword ptr [ebp - 0xe4]
        add eax, 0x0
        mov dword ptr [ebp - 0x5c], eax
        mov eax, dword ptr [ebp - 0x90]
        mov ebx, dword ptr [ebp - 0x94]
        mov ecx, dword ptr [ebp - 0x98]
        mov edx, dword ptr [ebp - 0x9c]
        mov esi, dword ptr [ebp - 0xa0]
        mov edi, dword ptr [ebp - 0xa4]
rotate_rle_43badc:
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        cmp ebx, eax
        jg rotate_rle_43baeb
        xchg eax, ebx
        xchg ecx, edx
        xchg esi, edi
rotate_rle_43baeb:
        sar eax, 0x10
        cmp eax, dword ptr [ebp - 0xdc]
        jg rotate_rle_43bdad
        sar ebx, 0x10
        cmp ebx, dword ptr [ebp - 0xd4]
        jl rotate_rle_43bdad
        mov dword ptr [ebp - 0xc0], eax
        mov dword ptr [ebp - 0xc4], ebx
        mov dword ptr [ebp - 0xd0], ecx
        sub ebx, eax
        je rotate_rle_43bbf0
        push ebx
        sub edx, ecx
        xor eax, eax
        shrd eax, edx, 0x10
        shl ebx, 0x10
        sar edx, 0x10
        idiv ebx
        mov dword ptr [ebp - 0xc8], eax
        shld edx, eax, 0x10
        pop ebx
        and eax, 0xffff
        and edx, 0xffff
        mov ecx, 0x1
        test edx, 0x8000
        je rotate_rle_43bb65
        or edx, 0xffff0000
        neg ecx
        cmp eax, 0x1
        sbb edx, -0x1
rotate_rle_43bb65:
        add ecx, edx
        push ecx
        push edx
        sub edi, esi
        mov edx, edi
        xor eax, eax
        shrd eax, edx, 0x10
        shl ebx, 0x10
        sar edx, 0x10
        idiv ebx
        mov dword ptr [ebp - 0xcc], eax
        shld edx, eax, 0x10
        and eax, 0xffff
        and edx, 0xffff
        mov ecx, dword ptr [ebp - 0x54]
        test edx, 0x8000
        je rotate_rle_43bba3
        neg ecx
        cmp eax, 0x1
        sbb edx, -0x1
rotate_rle_43bba3:
        mov eax, dword ptr [ebp - 0x54]
        imul dx
        cwde
        pop edx
        pop ebx
        add edx, eax
        mov dword ptr [anRLESourceSteps], edx
        add edx, ecx
        mov dword ptr [anRLESourceSteps + 0x4], edx
        add ebx, eax
        mov dword ptr [anRLESourceSteps + 0x8], ebx
        add ebx, ecx
        mov dword ptr [anRLESourceSteps + 0xc], ebx
        mov ecx, dword ptr [ebp - 0xd4]
        sub ecx, dword ptr [ebp - 0xc0]
        jg rotate_rle_43be33
rotate_rle_43bbde:
        mov eax, dword ptr [ebp - 0xc4]
        sub eax, dword ptr [ebp - 0xdc]
        jg rotate_rle_43be61
rotate_rle_43bbf0:
        mov ecx, esi
        shr esi, 0x10
        mov eax, esi
        mul dword ptr [ebp - 0x54]
        add eax, dword ptr [ebp - 0x58]
        mov esi, dword ptr [ebp - 0xd0]
        shr esi, 0x10
        add esi, eax
        mov eax, dword ptr [ebp - 0xc0]
        mov edi, dword ptr [ebp - 0x5c]
        add edi, eax
        mov ebx, dword ptr [ebp - 0xc4]
        sub ebx, eax
        push ebp
        mov edx, dword ptr [ebp - 0xd0]
        mov eax, dword ptr [ebp - 0xc8]
        or eax, eax
        jns rotate_rle_43bc30
        neg eax
        not edx
rotate_rle_43bc30:
        shl eax, 0x10
        shl edx, 0x10
        mov ebp, dword ptr [ebp - 0xcc]
        or ebp, ebp
        jns rotate_rle_43bc44
        neg ebp
        not ecx
rotate_rle_43bc44:
        shl ebp, 0x10
        shl ecx, 0x10
        push ebx
        xor ebx, ebx
        cmp dword ptr [esp], 0x5
        jl rotate_rle_43bd0f
rotate_rle_43bc57:
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bc60
        mov byte ptr [edi], bl
rotate_rle_43bc60:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bc7b
        mov byte ptr [edi + 0x1], bl
rotate_rle_43bc7b:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bc96
        mov byte ptr [edi + 0x2], bl
rotate_rle_43bc96:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bcb1
        mov byte ptr [edi + 0x3], bl
rotate_rle_43bcb1:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bccc
        mov byte ptr [edi + 0x4], bl
rotate_rle_43bccc:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bce7
        mov byte ptr [edi + 0x5], bl
rotate_rle_43bce7:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        add edi, 0x6
        sub dword ptr [esp], 0x6
        js rotate_rle_43bda9
        cmp dword ptr [esp], 0x5
        jge rotate_rle_43bc57
rotate_rle_43bd0f:
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bd18
        mov byte ptr [edi], bl
rotate_rle_43bd18:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        dec dword ptr [esp]
        js rotate_rle_43bda9
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bd38
        mov byte ptr [edi + 0x1], bl
rotate_rle_43bd38:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        dec dword ptr [esp]
        js rotate_rle_43bda9
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bd58
        mov byte ptr [edi + 0x2], bl
rotate_rle_43bd58:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        dec dword ptr [esp]
        js rotate_rle_43bda9
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bd78
        mov byte ptr [edi + 0x3], bl
rotate_rle_43bd78:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
        dec dword ptr [esp]
        js rotate_rle_43bda9
        mov bl, byte ptr [esi]
        cmp bl, -0x1
        je rotate_rle_43bd98
        mov byte ptr [edi + 0x4], bl
rotate_rle_43bd98:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr anRLESourceSteps[ebx*4]
rotate_rle_43bda9:
        add esp, 0x4
        pop ebp
rotate_rle_43bdad:
        mov edi, dword ptr [ebp - 0xe8]
        add dword ptr [ebp - 0x5c], edi
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        dec dword ptr [ebp - 0x88]
        js rotate_rle_43be04
        je rotate_rle_43be0a
        dec dword ptr [ebp - 0x80]
        je rotate_rle_43be6c
        add eax, dword ptr [ebp - 0xa8]
        add ecx, dword ptr [ebp - 0xb0]
        add esi, dword ptr [ebp - 0xb8]
rotate_rle_43bde1:
        dec dword ptr [ebp - 0x84]
        je rotate_rle_43bf1c
        add ebx, dword ptr [ebp - 0xac]
        add edx, dword ptr [ebp - 0xb4]
        add edi, dword ptr [ebp - 0xbc]
        jmp rotate_rle_43badc
rotate_rle_43be04:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
rotate_rle_43be0a:
        add eax, dword ptr [ebp - 0xa8]
        add ecx, dword ptr [ebp - 0xb0]
        add esi, dword ptr [ebp - 0xb8]
        add ebx, dword ptr [ebp - 0xac]
        add edx, dword ptr [ebp - 0xb4]
        add edi, dword ptr [ebp - 0xbc]
        jmp rotate_rle_43badc
rotate_rle_43be33:
        add dword ptr [ebp - 0xc0], ecx
        shl ecx, 0x10
        mov eax, dword ptr [ebp - 0xc8]
        imul ecx
        shrd eax, edx, 0x10
        add dword ptr [ebp - 0xd0], eax
        mov eax, dword ptr [ebp - 0xcc]
        imul ecx
        shrd eax, edx, 0x10
        add esi, eax
        jmp rotate_rle_43bbde
rotate_rle_43be61:
        sub dword ptr [ebp - 0xc4], eax
        jmp rotate_rle_43bbf0
rotate_rle_43be6c:
        push ebx
        push edx
        mov ebx, dword ptr [ebp - 0x78]
        mov dword ptr [ebp - 0x70], ebx
        mov esi, ebx
        sub esi, 0x14
        cmp esi, dword ptr [ebp - 0x64]
        jge rotate_rle_43be84
        mov esi, dword ptr [ebp - 0x68]
        sub esi, 0x14
rotate_rle_43be84:
        mov dword ptr [ebp - 0x78], esi
        mov ecx, dword ptr [esi + 0x4]
        mov edx, dword ptr [ebx + 0x4]
        sub ecx, edx
        cmp ecx, 0x1
        adc ecx, 0x0
        mov dword ptr [ebp - 0x80], ecx
        mov edx, dword ptr [esi]
        sub edx, dword ptr [ebx]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xa8], eax
        mov ecx, dword ptr [ebp - 0x80]
        mov edx, dword ptr [esi + 0xc]
        sub edx, dword ptr [ebx + 0xc]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xb0], eax
        mov ecx, dword ptr [ebp - 0x80]
        mov edx, dword ptr [esi + 0x10]
        sub edx, dword ptr [ebx + 0x10]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xb8], eax
        mov eax, dword ptr [ebx]
        shl eax, 0x10
        add eax, 0x8000
        mov ecx, dword ptr [ebx + 0xc]
        shl ecx, 0x10
        add ecx, 0x8000
        mov esi, dword ptr [ebx + 0x10]
        shl esi, 0x10
        add esi, 0x8000
        pop edx
        pop ebx
        jmp rotate_rle_43bde1
rotate_rle_43bf1c:
        push eax
        push ecx
        mov ebx, dword ptr [ebp - 0x7c]
        mov dword ptr [ebp - 0x74], ebx
        mov edi, ebx
        add edi, 0x14
        cmp edi, dword ptr [ebp - 0x68]
        jl rotate_rle_43bf31
        mov edi, dword ptr [ebp - 0x64]
rotate_rle_43bf31:
        mov dword ptr [ebp - 0x7c], edi
        mov ecx, dword ptr [edi + 0x4]
        mov edx, dword ptr [ebx + 0x4]
        sub ecx, edx
        cmp ecx, 0x1
        adc ecx, 0x0
        mov dword ptr [ebp - 0x84], ecx
        mov edx, dword ptr [edi]
        sub edx, dword ptr [ebx]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xac], eax
        mov ecx, dword ptr [ebp - 0x84]
        mov edx, dword ptr [edi + 0xc]
        sub edx, dword ptr [ebx + 0xc]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xb4], eax
        mov ecx, dword ptr [ebp - 0x84]
        mov edx, dword ptr [edi + 0x10]
        sub edx, dword ptr [ebx + 0x10]
        shl edx, 0x10
        xor eax, eax
        shrd eax, edx, 0x10
        shl ecx, 0x10
        sar edx, 0x10
        idiv ecx
        mov dword ptr [ebp - 0xbc], eax
        mov edx, dword ptr [ebx + 0xc]
        shl edx, 0x10
        add edx, 0x8000
        mov edi, dword ptr [ebx + 0x10]
        shl edi, 0x10
        add edi, 0x8000
        mov ebx, dword ptr [ebx]
        shl ebx, 0x10
        add ebx, 0x8000
        pop ecx
        pop eax
        jmp rotate_rle_43badc
rotate_rle_43bfd3:
        test dword ptr [ebp + 0x2c], 0x1
        je rotate_rle_43bff8
        push dword ptr [ebp + 0x18]
        push dword ptr [ebp + 0x14]
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call DrawRLEImageColor
        add esp, 0x14
        jmp rotate_rle_43be04
rotate_rle_43bff8:
        push dword ptr [ebp + 0x18]
        push dword ptr [ebp + 0x14]
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call DrawRLEImage
        add esp, 0x14
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
