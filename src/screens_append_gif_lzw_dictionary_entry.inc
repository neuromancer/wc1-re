    __asm {
        push ebx
        mov ebx, dword ptr [edi]
        mov word ptr [edi + ebx * 2 + 302eh], cx
        pop ebx
        push ebx
        mov al, byte ptr [ebx + edi + 102eh]
        mov ebx, dword ptr [edi]
        mov byte ptr [ebx + edi + 202eh], al
        mov ebx, ecx
        mov al, byte ptr [ebx + edi + 102eh]
        mov ebx, dword ptr [edi]
        mov byte ptr [ebx + edi + 102eh], al
        pop ebx
        inc dword ptr [edi]
        mov eax, dword ptr [edi]
        cmp eax, dword ptr [edi + 4]
        jne append_gif_lzw_done
        cmp dword ptr [edi + 1ch], 0ch
        jge append_gif_lzw_done
        inc dword ptr [edi + 1ch]
        shl dword ptr [edi + 4], 1
append_gif_lzw_done:
        ret
    }
