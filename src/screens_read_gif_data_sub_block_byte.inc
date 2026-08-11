    __asm {
        cmp dword ptr [edi + 10h], 0
        jne read_gif_sub_block_data
        lodsb
        and eax, 0ffh
        mov dword ptr [edi + 10h], eax
read_gif_sub_block_data:
        lodsb
        and eax, 0ffh
        dec dword ptr [edi + 10h]
        ret
    }
