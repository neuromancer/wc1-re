    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp + 8]
        mov al, byte ptr [esi + 0ah]
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        add esi, 0dh
        test al, 80h
        je gif_image_descriptor_ready
        imul ebx, ebx, 3
        add esi, ebx
gif_image_descriptor_ready:
        mov ax, word ptr [esi + 5]
        shl eax, 10h
        mov ax, word ptr [esi + 7]
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
