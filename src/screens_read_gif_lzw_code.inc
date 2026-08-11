    __asm {
        cmp dword ptr [edi + 18h], 0
        jne gif_lzw_have_bits
        call ReadGIFDataSubBlockByte
        mov dword ptr [edi + 14h], eax
        mov dword ptr [edi + 18h], 8
gif_lzw_have_bits:
        mov eax, edx
        cmp dword ptr [edi + 18h], eax
        jge gif_lzw_extract_code
        call ReadGIFDataSubBlockByte
        mov ecx, dword ptr [edi + 18h]
        shl eax, cl
        or dword ptr [edi + 14h], eax
        add dword ptr [edi + 18h], 8
gif_lzw_extract_code:
        mov ebx, edx
        movzx eax, byte ptr g_abGifLZWCodeMasks_0046ff15[ebx]
        mov ebx, dword ptr [edi + 14h]
        and ebx, eax
        push ebx
        sub dword ptr [edi + 18h], edx
        mov ecx, edx
        shr dword ptr [edi + 14h], cl
        pop eax
        ret
    }
