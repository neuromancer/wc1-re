    __asm {
        push ebp
        mov ebp, esp
        add esp, -0x58
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 0x8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 0x4]
        inc eax
        mov dword ptr [ebp - 0x50], eax
        jle draw_line_439eb8
        mov eax, dword ptr [ebx + 0x8]
        inc eax
        mov ecx, eax
        jle draw_line_439eb8
        mov eax, dword ptr [esi + 0x4]
        mov dword ptr [ebp - 0x54], eax
        cmp eax, 0x0
        jg draw_line_439e6c
        mov eax, 0x0
draw_line_439e6c:
        mov dword ptr [ebp - 0x3c], eax
        mov eax, dword ptr [esi + 0x8]
        mov dword ptr [ebp - 0x58], eax
        cmp eax, 0x0
        jg draw_line_439e7f
        mov eax, 0x0
draw_line_439e7f:
        mov dword ptr [ebp - 0x40], eax
        mov eax, dword ptr [esi + 0xc]
        mov edx, dword ptr [ebp - 0x50]
        dec edx
        cmp eax, edx
        jl draw_line_439e8f
        mov eax, edx
draw_line_439e8f:
        mov dword ptr [ebp - 0x44], eax
        mov eax, dword ptr [esi + 0x10]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl draw_line_439e9e
        mov eax, edx
draw_line_439e9e:
        mov dword ptr [ebp - 0x48], eax
        mov eax, dword ptr [ebp - 0x44]
        cmp eax, dword ptr [ebp - 0x3c]
        jl draw_line_439ec3
        mov eax, dword ptr [ebp - 0x48]
        cmp eax, dword ptr [ebp - 0x40]
        jl draw_line_439ec3
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 0x4c], eax
        jmp draw_line_439ece
draw_line_439eb8:
        mov eax, 0xffffffff
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
draw_line_439ec3:
        mov eax, 0xfffffffe
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
draw_line_439ece:
        mov eax, dword ptr [ebp - 0x54]
        add dword ptr [ebp + 0xc], eax
        add dword ptr [ebp + 0x14], eax
        mov eax, dword ptr [ebp - 0x58]
        add dword ptr [ebp + 0x10], eax
        add dword ptr [ebp + 0x18], eax
        mov eax, dword ptr [ebp + 0x14]
        sub eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x4], eax
        cdq
        mov dword ptr [ebp - 0xc], edx
        xor eax, edx
        sub eax, edx
        mov dword ptr [ebp - 0x8], eax
        mov eax, dword ptr [ebp + 0x18]
        sub eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x10], eax
        cdq
        mov dword ptr [ebp - 0x18], edx
        xor eax, edx
        sub eax, edx
        mov dword ptr [ebp - 0x14], eax
        mov eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x24], eax
        mov eax, dword ptr [ebp + 0x14]
        mov dword ptr [ebp - 0x2c], eax
        mov eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x28], eax
        mov eax, dword ptr [ebp + 0x18]
        mov dword ptr [ebp - 0x30], eax
        cmp dword ptr [ebp - 0x4], 0x0
        je draw_line_43a64f
        cmp dword ptr [ebp - 0x10], 0x0
        je draw_line_43a6da
        mov eax, dword ptr [ebp - 0xc]
        xor eax, dword ptr [ebp - 0x18]
        mov dword ptr [ebp - 0x1c], eax
        mov edx, dword ptr [ebp - 0x8]
        mov ebx, dword ptr [ebp - 0x14]
        mov eax, 0xffffffff
        cmp edx, ebx
        je draw_line_439f54
        jl draw_line_439f50
        xchg edx, ebx
draw_line_439f50:
        xor eax, eax
        div ebx
draw_line_439f54:
        mov dword ptr [ebp - 0x20], eax
        mov dword ptr [ebp - 0x34], 0x0
draw_line_439f5e:
        xor edx, edx
        mov eax, dword ptr [ebp - 0x24]
        sub eax, dword ptr [ebp - 0x3c]
        shl eax, 1
        _emit 0x12
        _emit 0xd2
        mov eax, dword ptr [ebp - 0x44]
        sub eax, dword ptr [ebp - 0x24]
        shl eax, 1
        _emit 0x12
        _emit 0xd2
        mov eax, dword ptr [ebp - 0x28]
        sub eax, dword ptr [ebp - 0x40]
        shl eax, 1
        _emit 0x12
        _emit 0xd2
        mov eax, dword ptr [ebp - 0x48]
        sub eax, dword ptr [ebp - 0x28]
        shl eax, 1
        _emit 0x12
        _emit 0xd2
        mov eax, dword ptr [ebp - 0x2c]
        sub eax, dword ptr [ebp - 0x3c]
        shl eax, 1
        _emit 0x12
        _emit 0xf6
        mov eax, dword ptr [ebp - 0x44]
        sub eax, dword ptr [ebp - 0x2c]
        shl eax, 1
        _emit 0x12
        _emit 0xf6
        mov eax, dword ptr [ebp - 0x30]
        sub eax, dword ptr [ebp - 0x40]
        shl eax, 1
        _emit 0x12
        _emit 0xf6
        mov eax, dword ptr [ebp - 0x48]
        sub eax, dword ptr [ebp - 0x30]
        shl eax, 1
        _emit 0x12
        _emit 0xf6
        or dword ptr [ebp - 0x34], edx
        or edx, edx
        je draw_line_43a31d
        test dh, dl
        jne draw_line_43a830
        mov ebx, dword ptr [ebp - 0x8]
        cmp ebx, dword ptr [ebp - 0x14]
        jl draw_line_43a018
        test dl, 0x8
        jne draw_line_43a061
        test dl, 0x4
        jne draw_line_43a0b5
        test dl, 0x2
        jne draw_line_43a139
        test dl, 0x1
        jne draw_line_43a191
        test dh, 0x8
        jne draw_line_43a1c1
        test dh, 0x4
        jne draw_line_43a21c
        test dh, 0x2
        jne draw_line_43a29b
        test dh, 0x1
        jne draw_line_43a2f2
        jmp draw_line_439f5e
draw_line_43a018:
        test dl, 0x8
        jne draw_line_43a089
        test dl, 0x4
        jne draw_line_43a0e1
        test dl, 0x2
        jne draw_line_43a111
        test dl, 0x1
        jne draw_line_43a165
        test dh, 0x8
        jne draw_line_43a1ed
        test dh, 0x4
        jne draw_line_43a244
        test dh, 0x2
        jne draw_line_43a26f
        test dh, 0x1
        jne draw_line_43a2ca
        jmp draw_line_439f5e
draw_line_43a061:
        mov eax, dword ptr [ebp - 0x3c]
        mov dword ptr [ebp - 0x24], eax
        sub eax, dword ptr [ebp + 0xc]
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x28], eax
        jmp draw_line_439f5e
draw_line_43a089:
        mov eax, dword ptr [ebp - 0x3c]
        mov dword ptr [ebp - 0x24], eax
        sub eax, dword ptr [ebp + 0xc]
        mov edx, eax
        dec edx
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, -0x1
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x28], eax
        jmp draw_line_439f5e
draw_line_43a0b5:
        mov eax, dword ptr [ebp - 0x44]
        mov dword ptr [ebp - 0x24], eax
        sub eax, dword ptr [ebp + 0xc]
        neg eax
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x28], eax
        jmp draw_line_439f5e
draw_line_43a0e1:
        mov eax, dword ptr [ebp - 0x44]
        mov dword ptr [ebp - 0x24], eax
        sub eax, dword ptr [ebp + 0xc]
        neg eax
        mov edx, eax
        dec edx
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, -0x1
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x28], eax
        jmp draw_line_439f5e
draw_line_43a111:
        mov eax, dword ptr [ebp - 0x40]
        mov dword ptr [ebp - 0x28], eax
        sub eax, dword ptr [ebp + 0x10]
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x24], eax
        jmp draw_line_439f5e
draw_line_43a139:
        mov eax, dword ptr [ebp - 0x40]
        mov dword ptr [ebp - 0x28], eax
        sub eax, dword ptr [ebp + 0x10]
        mov edx, eax
        dec edx
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, -0x1
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x24], eax
        jmp draw_line_439f5e
draw_line_43a165:
        mov eax, dword ptr [ebp - 0x48]
        mov dword ptr [ebp - 0x28], eax
        sub eax, dword ptr [ebp + 0x10]
        neg eax
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x24], eax
        jmp draw_line_439f5e
draw_line_43a191:
        mov eax, dword ptr [ebp - 0x48]
        mov dword ptr [ebp - 0x28], eax
        sub eax, dword ptr [ebp + 0x10]
        neg eax
        mov edx, eax
        dec edx
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, -0x1
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x24], eax
        jmp draw_line_439f5e
draw_line_43a1c1:
        mov eax, dword ptr [ebp - 0x3c]
        mov dword ptr [ebp - 0x2c], eax
        sub eax, dword ptr [ebp + 0xc]
        neg eax
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x30], eax
        jmp draw_line_439f5e
draw_line_43a1ed:
        mov eax, dword ptr [ebp - 0x3c]
        mov dword ptr [ebp - 0x2c], eax
        sub eax, dword ptr [ebp + 0xc]
        neg eax
        mov edx, eax
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, 0x0
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x30], eax
        jmp draw_line_439f5e
draw_line_43a21c:
        mov eax, dword ptr [ebp - 0x44]
        mov dword ptr [ebp - 0x2c], eax
        sub eax, dword ptr [ebp + 0xc]
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x30], eax
        jmp draw_line_439f5e
draw_line_43a244:
        mov eax, dword ptr [ebp - 0x44]
        mov dword ptr [ebp - 0x2c], eax
        sub eax, dword ptr [ebp + 0xc]
        mov edx, eax
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, 0x0
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x30], eax
        jmp draw_line_439f5e
draw_line_43a26f:
        mov eax, dword ptr [ebp - 0x40]
        mov dword ptr [ebp - 0x30], eax
        sub eax, dword ptr [ebp + 0x10]
        neg eax
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x2c], eax
        jmp draw_line_439f5e
draw_line_43a29b:
        mov eax, dword ptr [ebp - 0x40]
        mov dword ptr [ebp - 0x30], eax
        sub eax, dword ptr [ebp + 0x10]
        neg eax
        mov edx, eax
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, 0x0
        mov edx, dword ptr [ebp - 0x1c]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x2c], eax
        jmp draw_line_439f5e
draw_line_43a2ca:
        mov eax, dword ptr [ebp - 0x48]
        mov dword ptr [ebp - 0x30], eax
        sub eax, dword ptr [ebp + 0x10]
        mul dword ptr [ebp - 0x20]
        add eax, 0x80000000
        adc edx, 0x0
        mov eax, edx
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x2c], eax
        jmp draw_line_439f5e
draw_line_43a2f2:
        mov eax, dword ptr [ebp - 0x48]
        mov dword ptr [ebp - 0x30], eax
        sub eax, dword ptr [ebp + 0x10]
        mov edx, eax
        mov eax, 0x80000000
        div dword ptr [ebp - 0x20]
        cmp edx, 0x1
        sbb eax, 0x0
        mov edx, dword ptr [ebp - 0x1c]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x2c], eax
        jmp draw_line_439f5e
draw_line_43a31d:
        mov eax, dword ptr [ebp - 0x28]
        imul dword ptr [ebp - 0x50]
        add eax, dword ptr [ebp - 0x4c]
        add eax, dword ptr [ebp - 0x24]
        mov edi, eax
        mov esi, dword ptr [ebp - 0x50]
        xor esi, dword ptr [ebp - 0x18]
        sub esi, dword ptr [ebp - 0x18]
        mov ebx, dword ptr [ebp - 0x20]
        mov eax, dword ptr [ebp - 0x8]
        cmp eax, dword ptr [ebp - 0x14]
        je draw_line_43a760
        jg draw_line_43a4b1
        mov eax, dword ptr [ebp - 0x30]
        sub eax, dword ptr [ebp - 0x28]
        cdq
        xor eax, edx
        sub eax, edx
        inc eax
        mov ecx, eax
        mov eax, dword ptr [ebp - 0x28]
        sub eax, dword ptr [ebp + 0x10]
        cdq
        xor eax, edx
        sub eax, edx
        mul ebx
        add eax, 0x80000000
        mov edx, eax
        cmp dword ptr [ebp - 0xc], -0x1
        je draw_line_43a412
        cmp dword ptr [ebp + 0x1c], 0x1
        je draw_line_43a3a9
        jg draw_line_43a3e7
        mov eax, dword ptr [ebp + 0x20]
draw_line_43a380:
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        je draw_line_43a3a4
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        je draw_line_43a3a4
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        je draw_line_43a3a4
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        jne draw_line_43a380
draw_line_43a3a4:
        jmp draw_line_43a821
draw_line_43a3a9:
        mov eax, dword ptr [ebp + 0x20]
        push ebp
        mov ebp, ebx
        mov ebx, eax
draw_line_43a3b1:
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        je draw_line_43a3e1
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        je draw_line_43a3e1
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        je draw_line_43a3e1
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        jne draw_line_43a3b1
draw_line_43a3e1:
        pop ebp
        jmp draw_line_43a821
draw_line_43a3e7:
        mov esi, dword ptr [ebp + 0x8]
        mov edi, dword ptr [ebp - 0x24]
        sub edi, dword ptr [esi + 0x4]
        mov eax, dword ptr [ebp - 0x28]
        sub eax, dword ptr [esi + 0x8]
        mov esi, eax
        mov eax, dword ptr [ebp - 0x18]
        add eax, eax
        inc eax
draw_line_43a3fe:
        pushad
        call dword ptr [ebp + 0x20]
        popad
        add edx, ebx
        jae draw_line_43a408
        inc edi
draw_line_43a408:
        add esi, eax
        dec ecx
        jne draw_line_43a3fe
        jmp draw_line_43a821
draw_line_43a412:
        neg esi
        cmp dword ptr [ebp + 0x1c], 0x1
        je draw_line_43a448
        jg draw_line_43a486
        mov eax, dword ptr [ebp + 0x20]
draw_line_43a41f:
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        je draw_line_43a443
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        je draw_line_43a443
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        je draw_line_43a443
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        jne draw_line_43a41f
draw_line_43a443:
        jmp draw_line_43a821
draw_line_43a448:
        mov eax, dword ptr [ebp + 0x20]
        push ebp
        mov ebp, ebx
        mov ebx, eax
draw_line_43a450:
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        je draw_line_43a480
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        je draw_line_43a480
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        je draw_line_43a480
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        jne draw_line_43a450
draw_line_43a480:
        pop ebp
        jmp draw_line_43a821
draw_line_43a486:
        mov esi, dword ptr [ebp + 0x8]
        mov edi, dword ptr [ebp - 0x24]
        sub edi, dword ptr [esi + 0x4]
        mov eax, dword ptr [ebp - 0x28]
        sub eax, dword ptr [esi + 0x8]
        mov esi, eax
        mov eax, dword ptr [ebp - 0x18]
        add eax, eax
        inc eax
draw_line_43a49d:
        pushad
        call dword ptr [ebp + 0x20]
        popad
        add edx, ebx
        jae draw_line_43a4a7
        dec edi
draw_line_43a4a7:
        add esi, eax
        dec ecx
        jne draw_line_43a49d
        jmp draw_line_43a821
draw_line_43a4b1:
        mov eax, dword ptr [ebp - 0x2c]
        sub eax, dword ptr [ebp - 0x24]
        cdq
        xor eax, edx
        sub eax, edx
        inc eax
        mov ecx, eax
        mov eax, dword ptr [ebp - 0x24]
        sub eax, dword ptr [ebp + 0xc]
        cdq
        xor eax, edx
        sub eax, edx
        mul ebx
        add eax, 0x80000000
        mov edx, eax
        cmp dword ptr [ebp - 0xc], -0x1
        je draw_line_43a596
        cmp dword ptr [ebp + 0x1c], 0x1
        je draw_line_43a521
        jg draw_line_43a56b
        mov eax, dword ptr [ebp + 0x20]
draw_line_43a4ec:
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae draw_line_43a4f5
        add edi, esi
draw_line_43a4f5:
        dec ecx
        je draw_line_43a51c
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae draw_line_43a501
        add edi, esi
draw_line_43a501:
        dec ecx
        je draw_line_43a51c
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae draw_line_43a50d
        add edi, esi
draw_line_43a50d:
        dec ecx
        je draw_line_43a51c
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae draw_line_43a519
        add edi, esi
draw_line_43a519:
        dec ecx
        jne draw_line_43a4ec
draw_line_43a51c:
        jmp draw_line_43a821
draw_line_43a521:
        mov eax, dword ptr [ebp + 0x20]
        push ebp
        mov ebp, ebx
        mov ebx, eax
draw_line_43a529:
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae draw_line_43a535
        add edi, esi
draw_line_43a535:
        dec ecx
        je draw_line_43a565
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae draw_line_43a544
        add edi, esi
draw_line_43a544:
        dec ecx
        je draw_line_43a565
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae draw_line_43a553
        add edi, esi
draw_line_43a553:
        dec ecx
        je draw_line_43a565
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae draw_line_43a562
        add edi, esi
draw_line_43a562:
        dec ecx
        jne draw_line_43a529
draw_line_43a565:
        pop ebp
        jmp draw_line_43a821
draw_line_43a56b:
        mov esi, dword ptr [ebp + 0x8]
        mov edi, dword ptr [ebp - 0x24]
        sub edi, dword ptr [esi + 0x4]
        mov eax, dword ptr [ebp - 0x28]
        sub eax, dword ptr [esi + 0x8]
        mov esi, eax
        mov eax, dword ptr [ebp - 0x18]
        add eax, eax
        inc eax
draw_line_43a582:
        pushad
        call dword ptr [ebp + 0x20]
        popad
        add edx, ebx
        jae draw_line_43a58c
        inc esi
draw_line_43a58c:
        add edi, eax
        dec ecx
        jne draw_line_43a582
        jmp draw_line_43a821
draw_line_43a596:
        cmp dword ptr [ebp + 0x1c], 0x1
        je draw_line_43a5da
        jg draw_line_43a624
        mov eax, dword ptr [ebp + 0x20]
draw_line_43a5a5:
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae draw_line_43a5ae
        add edi, esi
draw_line_43a5ae:
        dec ecx
        je draw_line_43a5d5
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae draw_line_43a5ba
        add edi, esi
draw_line_43a5ba:
        dec ecx
        je draw_line_43a5d5
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae draw_line_43a5c6
        add edi, esi
draw_line_43a5c6:
        dec ecx
        je draw_line_43a5d5
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae draw_line_43a5d2
        add edi, esi
draw_line_43a5d2:
        dec ecx
        jne draw_line_43a5a5
draw_line_43a5d5:
        jmp draw_line_43a821
draw_line_43a5da:
        mov eax, dword ptr [ebp + 0x20]
        push ebp
        mov ebp, ebx
        mov ebx, eax
draw_line_43a5e2:
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae draw_line_43a5ee
        add edi, esi
draw_line_43a5ee:
        dec ecx
        je draw_line_43a61e
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae draw_line_43a5fd
        add edi, esi
draw_line_43a5fd:
        dec ecx
        je draw_line_43a61e
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae draw_line_43a60c
        add edi, esi
draw_line_43a60c:
        dec ecx
        je draw_line_43a61e
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae draw_line_43a61b
        add edi, esi
draw_line_43a61b:
        dec ecx
        jne draw_line_43a5e2
draw_line_43a61e:
        pop ebp
        jmp draw_line_43a821
draw_line_43a624:
        mov esi, dword ptr [ebp + 0x8]
        mov edi, dword ptr [ebp - 0x24]
        sub edi, dword ptr [esi + 0x4]
        mov eax, dword ptr [ebp - 0x28]
        sub eax, dword ptr [esi + 0x8]
        mov esi, eax
        mov eax, dword ptr [ebp - 0x18]
        add eax, eax
        inc eax
draw_line_43a63b:
        pushad
        call dword ptr [ebp + 0x20]
        popad
        add edx, ebx
        jae draw_line_43a645
        dec esi
draw_line_43a645:
        add edi, eax
        dec ecx
        jne draw_line_43a63b
        jmp draw_line_43a821
draw_line_43a64f:
        mov eax, dword ptr [ebp + 0xc]
        cmp eax, dword ptr [ebp - 0x3c]
        jl draw_line_43a830
        cmp eax, dword ptr [ebp - 0x44]
        jg draw_line_43a830
        mov eax, dword ptr [ebp + 0x10]
        cmp eax, dword ptr [ebp + 0x18]
        jg draw_line_43a66f
        mov eax, dword ptr [ebp + 0x18]
draw_line_43a66f:
        cmp eax, dword ptr [ebp - 0x40]
        jl draw_line_43a830
        mov eax, dword ptr [ebp + 0x10]
        cmp eax, dword ptr [ebp + 0x18]
        jl draw_line_43a683
        mov eax, dword ptr [ebp + 0x18]
draw_line_43a683:
        cmp eax, dword ptr [ebp - 0x48]
        jg draw_line_43a830
        mov eax, dword ptr [ebp + 0x10]
        cmp eax, dword ptr [ebp - 0x40]
        jg draw_line_43a697
        mov eax, dword ptr [ebp - 0x40]
draw_line_43a697:
        cmp eax, dword ptr [ebp - 0x48]
        jl draw_line_43a69f
        mov eax, dword ptr [ebp - 0x48]
draw_line_43a69f:
        mov dword ptr [ebp - 0x28], eax
        mov eax, dword ptr [ebp + 0x18]
        cmp eax, dword ptr [ebp - 0x40]
        jg draw_line_43a6ad
        mov eax, dword ptr [ebp - 0x40]
draw_line_43a6ad:
        cmp eax, dword ptr [ebp - 0x48]
        jl draw_line_43a6b5
        mov eax, dword ptr [ebp - 0x48]
draw_line_43a6b5:
        mov dword ptr [ebp - 0x30], eax
        mov eax, dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x24], eax
        mov esi, dword ptr [ebp - 0x50]
        xor esi, dword ptr [ebp - 0x18]
        sub esi, dword ptr [ebp - 0x18]
        mov eax, dword ptr [ebp - 0x30]
        sub eax, dword ptr [ebp - 0x28]
        cdq
        xor eax, edx
        sub eax, edx
        mov ecx, eax
        inc ecx
        jmp draw_line_43a780
draw_line_43a6da:
        mov eax, dword ptr [ebp + 0x10]
        cmp eax, dword ptr [ebp - 0x40]
        jl draw_line_43a830
        cmp eax, dword ptr [ebp - 0x48]
        jg draw_line_43a830
        mov eax, dword ptr [ebp + 0xc]
        cmp eax, dword ptr [ebp + 0x14]
        jg draw_line_43a6fa
        mov eax, dword ptr [ebp + 0x14]
draw_line_43a6fa:
        cmp eax, dword ptr [ebp - 0x3c]
        jl draw_line_43a830
        mov eax, dword ptr [ebp + 0xc]
        cmp eax, dword ptr [ebp + 0x14]
        jl draw_line_43a70e
        mov eax, dword ptr [ebp + 0x14]
draw_line_43a70e:
        cmp eax, dword ptr [ebp - 0x44]
        jg draw_line_43a830
        mov eax, dword ptr [ebp + 0xc]
        cmp eax, dword ptr [ebp - 0x3c]
        jg draw_line_43a722
        mov eax, dword ptr [ebp - 0x3c]
draw_line_43a722:
        cmp eax, dword ptr [ebp - 0x44]
        jl draw_line_43a72a
        mov eax, dword ptr [ebp - 0x44]
draw_line_43a72a:
        mov dword ptr [ebp - 0x24], eax
        mov eax, dword ptr [ebp + 0x14]
        cmp eax, dword ptr [ebp - 0x3c]
        jg draw_line_43a738
        mov eax, dword ptr [ebp - 0x3c]
draw_line_43a738:
        cmp eax, dword ptr [ebp - 0x44]
        jl draw_line_43a740
        mov eax, dword ptr [ebp - 0x44]
draw_line_43a740:
        mov dword ptr [ebp - 0x2c], eax
        mov eax, dword ptr [ebp + 0x10]
        mov dword ptr [ebp - 0x28], eax
        mov esi, dword ptr [ebp - 0xc]
        inc esi
        or esi, dword ptr [ebp - 0xc]
        mov eax, dword ptr [ebp - 0x2c]
        sub eax, dword ptr [ebp - 0x24]
        cdq
        xor eax, edx
        sub eax, edx
        mov ecx, eax
        inc ecx
        jmp draw_line_43a780
draw_line_43a760:
        mov esi, dword ptr [ebp - 0x50]
        xor esi, dword ptr [ebp - 0x18]
        sub esi, dword ptr [ebp - 0x18]
        mov eax, dword ptr [ebp - 0xc]
        inc eax
        or eax, dword ptr [ebp - 0xc]
        add esi, eax
        mov eax, dword ptr [ebp - 0x2c]
        sub eax, dword ptr [ebp - 0x24]
        cdq
        xor eax, edx
        sub eax, edx
        mov ecx, eax
        inc ecx
draw_line_43a780:
        mov eax, dword ptr [ebp - 0x28]
        imul dword ptr [ebp - 0x50]
        add eax, dword ptr [ebp - 0x4c]
        add eax, dword ptr [ebp - 0x24]
        mov edi, eax
        cmp dword ptr [ebp + 0x1c], 0x1
        je draw_line_43a7b7
        jg draw_line_43a7e4
        mov eax, dword ptr [ebp + 0x20]
draw_line_43a799:
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je draw_line_43a7b5
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je draw_line_43a7b5
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je draw_line_43a7b5
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        jne draw_line_43a799
draw_line_43a7b5:
        jmp draw_line_43a821
draw_line_43a7b7:
        mov ebx, dword ptr [ebp + 0x20]
draw_line_43a7ba:
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je draw_line_43a7e2
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je draw_line_43a7e2
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je draw_line_43a7e2
        mov al, byte ptr [edi]
        xlat
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        jne draw_line_43a7ba
draw_line_43a7e2:
        jmp draw_line_43a821
draw_line_43a7e4:
        mov esi, dword ptr [ebp + 0x8]
        mov edi, dword ptr [ebp - 0x24]
        sub edi, dword ptr [esi + 0x4]
        mov eax, dword ptr [ebp - 0x28]
        sub eax, dword ptr [esi + 0x8]
        mov esi, eax
        xor eax, eax
        test dword ptr [ebp - 0x10], 0xffffffff
        setne al
        or eax, dword ptr [ebp - 0x18]
        xor ebx, ebx
        test dword ptr [ebp - 0x4], 0xffffffff
        setne bl
        or ebx, dword ptr [ebp - 0xc]
draw_line_43a813:
        pushad
        call dword ptr [ebp + 0x20]
        popad
        add esi, eax
        add edi, ebx
        dec ecx
        jne draw_line_43a813
        jmp draw_line_43a821
draw_line_43a821:
        xor eax, eax
        cmp dword ptr [ebp - 0x34], 0x1
        setae al
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
draw_line_43a830:
        mov eax, 0x2
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
