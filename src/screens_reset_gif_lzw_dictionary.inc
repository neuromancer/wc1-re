    __asm {
        mov ebx, 0
        mov eax, ecx
        add eax, 2
        mov dword ptr [edi], eax
        mov eax, ecx
        shl eax, 1
        mov dword ptr [edi + 4], eax
reset_gif_lzw_literal:
        cmp ebx, ecx
        jge reset_gif_lzw_unused
        mov byte ptr [ebx + edi + 102eh], bl
        mov byte ptr [ebx + edi + 202eh], bl
        mov word ptr [edi + ebx * 2 + 302eh], 0ffffh
        inc ebx
        jmp reset_gif_lzw_literal
reset_gif_lzw_unused:
        cmp ebx, 1000h
        jge reset_gif_lzw_done
        mov word ptr [edi + ebx * 2 + 302eh], 0fffeh
        inc ebx
        jmp reset_gif_lzw_unused
reset_gif_lzw_done:
        ret
    }
