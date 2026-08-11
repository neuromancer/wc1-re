    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov al, byte ptr [esi + 0ah]
        add esi, 0dh
        test al, 80h
        je gif_global_palette_done
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        imul ebx, ebx, 3
        mov edi, dword ptr [ebp + 0ch]
        mov ecx, ebx
copy_gif_global_palette_component:
        lodsb
        shr al, 2
        stosb
        loop copy_gif_global_palette_component
gif_global_palette_done:
        mov al, byte ptr [esi + 9]
        test al, 80h
        je gif_palette_done
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        imul ebx, ebx, 3
        add esi, 0ah
        mov edi, dword ptr [ebp + 0ch]
        mov ecx, ebx
copy_gif_local_palette_component:
        lodsb
        shr al, 2
        stosb
        loop copy_gif_local_palette_component
gif_palette_done:
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
