; @file
; @ingroup ixlib
; @brief Hand-written software sample mixer.
;
; Address range 446f74-4471ff, originally assembled as a separate object.
; The retail routine uses MASM's D1 shift-by-one encodings and short
; branches. INT3 fill before it aligns the object to four bytes.
; Keep the C++ names of the shared state; the callable entry uses C linkage.

        .386
        .model flat, c
        option casemap:none

        extern syscall ?aVoices@@3PAUIxVoice@@A:byte
        extern syscall ?nVoiceCount@@3HA:dword
        extern syscall ?nStreamCount@@3HA:dword
        public ix_dspv_mix

        .code

; Function start: 0x00446F74
; void ix_dspv_mix(void *outputBuffer, unsigned int outputBytes);
ix_dspv_mix proc
        push ebp
        mov ebp, esp
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        mov eax, 0
        mov edi, dword ptr [ebp + 8]
        mov ecx, dword ptr [ebp + 0Ch]
        shr ecx, 2
        rep stosd
        mov ebx, offset ?aVoices@@3PAUIxVoice@@A
        mov ecx, dword ptr [?nVoiceCount@@3HA]
        add ecx, dword ptr [?nStreamCount@@3HA]

    voice_loop:
        mov eax, dword ptr [ebx]
        test eax, 2
        jz next_voice
        push ecx
        mov edi, dword ptr [ebp + 8]
        mov ecx, dword ptr [ebp + 0Ch]
        shr ecx, 2
        mov esi, dword ptr [ebx + 4]
        test eax, 8
        jnz mix_16bit
        test eax, 10h
        jnz mix_8bit_stereo

    mix_8bit_mono_sample:
        mov al, byte ptr [esi]
        mov dl, al
        imul byte ptr [ebx + 17h]
        rol eax, 10h
        mov al, dl
        imul byte ptr [ebx + 16h]

    mix_8bit_mono_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_8bit_mono_left_done
        jnc mix_8bit_mono_left_positive
        mov ax, 8000h
        jmp mix_8bit_mono_left_done

    mix_8bit_mono_left_positive:
        mov ax, 7FFFh

    mix_8bit_mono_left_done:
        ror eax, 10h
        ror edx, 10h
        add ax, dx
        jno mix_8bit_mono_right_done
        jnc mix_8bit_mono_right_positive
        mov ax, 8000h
        jmp mix_8bit_mono_right_done

    mix_8bit_mono_right_positive:
        mov ax, 7FFFh

    mix_8bit_mono_right_done:
        rol eax, 10h
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 10h]
        add dx, word ptr [ebx + 12h]
        mov word ptr [ebx + 10h], dx
        and word ptr [ebx + 10h], 0FFh
        shr dx, 8
        jz mix_8bit_mono_accumulate
        add esi, edx
        cmp esi, dword ptr [ebx + 0Ch]
        jl mix_8bit_mono_sample
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_8bit_mono_sample

    mix_8bit_stereo:
        cmp esi, dword ptr [ebx + 0Ch]
        jge mix_8bit_stereo_at_end

    mix_8bit_stereo_sample:
        mov al, byte ptr [esi + 1]
        imul byte ptr [ebx + 17h]
        rol eax, 10h
        mov al, byte ptr [esi]
        imul byte ptr [ebx + 16h]

    mix_8bit_stereo_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_8bit_stereo_left_done
        jnc mix_8bit_stereo_left_positive
        mov ax, 8000h
        jmp mix_8bit_stereo_left_done

    mix_8bit_stereo_left_positive:
        mov ax, 7FFFh

    mix_8bit_stereo_left_done:
        ror eax, 10h
        ror edx, 10h
        add ax, dx
        jno mix_8bit_stereo_right_done
        jnc mix_8bit_stereo_right_positive
        mov ax, 8000h
        jmp mix_8bit_stereo_right_done

    mix_8bit_stereo_right_positive:
        mov ax, 7FFFh

    mix_8bit_stereo_right_done:
        rol eax, 10h
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 10h]
        add dx, word ptr [ebx + 12h]
        mov word ptr [ebx + 10h], dx
        and word ptr [ebx + 10h], 0FFh
        shr dx, 8
        jz mix_8bit_stereo_accumulate
        shl dx, 1
        add esi, edx
        jmp mix_8bit_stereo

    mix_8bit_stereo_at_end:
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_8bit_stereo_sample

    mix_16bit:
        test eax, 10h
        jnz mix_16bit_stereo

    mix_16bit_mono:
        cmp esi, dword ptr [ebx + 0Ch]
        jge mix_16bit_mono_at_end

    mix_16bit_mono_sample:
        mov ax, word ptr [esi]
        push ax
        imul word ptr [ebx + 1Ah]
        mov ax, dx
        rol eax, 10h
        pop ax
        imul word ptr [ebx + 18h]
        mov ax, dx

    mix_16bit_mono_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_16bit_mono_left_done
        jnc mix_16bit_mono_left_positive
        mov ax, 8000h
        jmp mix_16bit_mono_left_done

    mix_16bit_mono_left_positive:
        mov ax, 7FFFh

    mix_16bit_mono_left_done:
        ror eax, 10h
        ror edx, 10h
        add ax, dx
        jno mix_16bit_mono_right_done
        jnc mix_16bit_mono_right_positive
        mov ax, 8000h
        jmp mix_16bit_mono_right_done

    mix_16bit_mono_right_positive:
        mov ax, 7FFFh

    mix_16bit_mono_right_done:
        rol eax, 10h
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 10h]
        add dx, word ptr [ebx + 12h]
        mov word ptr [ebx + 10h], dx
        and word ptr [ebx + 10h], 0FFh
        shr dx, 8
        jz mix_16bit_mono_accumulate
        shl dx, 1
        add esi, edx
        jmp mix_16bit_mono

    mix_16bit_mono_at_end:
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_16bit_mono_sample

    mix_16bit_stereo:
        cmp esi, dword ptr [ebx + 0Ch]
        jge mix_16bit_stereo_at_end

    mix_16bit_stereo_sample:
        mov ax, word ptr [esi + 2]
        imul word ptr [ebx + 1Ah]
        mov ax, dx
        rol eax, 10h
        mov ax, word ptr [esi]
        imul word ptr [ebx + 18h]
        mov ax, dx

    mix_16bit_stereo_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_16bit_stereo_left_done
        jnc mix_16bit_stereo_left_positive
        mov ax, 8000h
        jmp mix_16bit_stereo_left_done

    mix_16bit_stereo_left_positive:
        mov ax, 7FFFh

    mix_16bit_stereo_left_done:
        ror eax, 10h
        ror edx, 10h
        add ax, dx
        jno mix_16bit_stereo_right_done
        jnc mix_16bit_stereo_right_positive
        mov ax, 8000h
        jmp mix_16bit_stereo_right_done

    mix_16bit_stereo_right_positive:
        mov ax, 7FFFh

    mix_16bit_stereo_right_done:
        rol eax, 10h
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 10h]
        add dx, word ptr [ebx + 12h]
        mov word ptr [ebx + 10h], dx
        and word ptr [ebx + 10h], 0FFh
        shr dx, 8
        jz mix_16bit_stereo_accumulate
        shl dx, 2
        add esi, edx
        jmp mix_16bit_stereo

    mix_16bit_stereo_at_end:
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_16bit_stereo_sample

    stop_voice:
        and dword ptr [ebx], 0FFFFFFFDh

    voice_done:
        mov dword ptr [ebx + 4], esi
        pop ecx

    next_voice:
        dec ecx
        jz mix_done
        lea ebx, [ebx + 20h]
        jmp voice_loop

    mix_done:
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        leave
        ret
ix_dspv_mix endp

        end
