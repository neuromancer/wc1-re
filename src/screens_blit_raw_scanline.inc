    __asm {
        push ebp
        mov ebp, esp
        add esp, -24h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 1ch], eax
        jle raw_scanline_invalid_surface
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle raw_scanline_invalid_surface
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 20h], eax
        cmp eax, 0
        jg raw_scanline_left_ready
        mov eax, 0
raw_scanline_left_ready:
        mov dword ptr [ebp - 8], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 24h], eax
        cmp eax, 0
        jg raw_scanline_top_ready
        mov eax, 0
raw_scanline_top_ready:
        mov dword ptr [ebp - 0ch], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 1ch]
        dec edx
        cmp eax, edx
        jl raw_scanline_right_ready
        mov eax, edx
raw_scanline_right_ready:
        mov dword ptr [ebp - 10h], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl raw_scanline_bottom_ready
        mov eax, edx
raw_scanline_bottom_ready:
        mov dword ptr [ebp - 14h], eax
        mov eax, dword ptr [ebp - 10h]
        cmp eax, dword ptr [ebp - 8]
        jl raw_scanline_empty_clip
        mov eax, dword ptr [ebp - 14h]
        cmp eax, dword ptr [ebp - 0ch]
        jl raw_scanline_empty_clip
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 18h], eax
        jmp raw_scanline_clip_span
raw_scanline_invalid_surface:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
raw_scanline_empty_clip:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
raw_scanline_clip_span:
        mov dword ptr [ebp - 4], 0
        mov eax, dword ptr [ebp - 20h]
        add dword ptr [ebp - 4], eax
        mov eax, dword ptr [ebp - 24h]
        add dword ptr [ebp + 0ch], eax
        mov esi, dword ptr [ebp + 10h]
        mov edi, dword ptr [ebp + 8]
        mov ecx, dword ptr [ebp + 14h]
        mov eax, dword ptr [ebp - 10h]
        sub eax, dword ptr [ebp - 4]
        inc eax
        sub eax, ecx
        jns raw_scanline_clip_left
        add ecx, eax
        jle raw_scanline_done
raw_scanline_clip_left:
        mov eax, dword ptr [ebp - 4]
        sub eax, dword ptr [ebp - 8]
        jns raw_scanline_clip_y
        add ecx, eax
        jle raw_scanline_done
        sub esi, eax
        sub dword ptr [ebp - 4], eax
raw_scanline_clip_y:
        mov eax, dword ptr [ebp - 14h]
        sub eax, dword ptr [ebp + 0ch]
        js raw_scanline_done
        mov eax, dword ptr [ebp + 0ch]
        sub eax, dword ptr [ebp - 0ch]
        js raw_scanline_done
        mov eax, dword ptr [ebp + 0ch]
        imul dword ptr [ebp - 1ch]
        add eax, dword ptr [ebp - 18h]
        add eax, dword ptr [ebp - 4]
        mov edi, eax
        mov edx, ecx
        and ecx, 3
        rep movsb
        mov ecx, edx
        shr ecx, 2
        rep movsd
raw_scanline_done:
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
