    __asm {
        mov ebx, dword ptr [edi + 8]
        mov byte ptr g_abRasterDecodeBuffer_0046ec15[ebx], al
        inc dword ptr [edi + 8]
        dec dword ptr [edi + 20h]
        cmp dword ptr [edi + 20h], 0
        jne emit_gif_pixel_done
        push dword ptr [edi + 24h]
        push offset g_abRasterDecodeBuffer_0046ec15
        push dword ptr [edi + 0ch]
        push dword ptr g_pGifRasterTarget_0046ff28
        call BlitRawScanline
        add esp, 10h
        mov dword ptr [edi + 8], 0
        mov eax, dword ptr [edi + 24h]
        mov dword ptr [edi + 20h], eax
        cmp byte ptr [edi + 2ch], 0
        je emit_gif_noninterlaced_row
        movzx ebx, byte ptr [edi + 2dh]
        movzx eax, byte ptr g_abGifInterlaceRowSteps_0046ff1e[ebx]
        add dword ptr [edi + 0ch], eax
        mov eax, dword ptr [edi + 0ch]
        cmp eax, dword ptr [edi + 28h]
        jl emit_gif_pixel_done
        inc byte ptr [edi + 2dh]
        movzx ebx, byte ptr [edi + 2dh]
        movzx eax, byte ptr g_abGifInterlaceRowStarts_0046ff23[ebx]
        mov dword ptr [edi + 0ch], eax
        jmp emit_gif_pixel_done
emit_gif_noninterlaced_row:
        inc dword ptr [edi + 0ch]
        mov eax, dword ptr [edi + 0ch]
        cmp eax, dword ptr [edi + 28h]
        jl emit_gif_pixel_done
        mov dword ptr [edi + 0ch], 0
emit_gif_pixel_done:
        ret
    }
