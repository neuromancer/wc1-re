/*
 * LZO1X decoder used by the packed ix streamer.
 *
 * The retail routine is hand-written x86: it keeps input/token state in EBX
 * and EDX and performs overlapping copies with REP MOVS.  Preserve that code
 * directly so packed stream chunks decode exactly as they did originally.
 */

/* Function start: 0x004614C0 */
extern "C" __declspec(naked) void ix_lzo1x_decompress(
    unsigned char *source, unsigned char *destination,
    unsigned int destinationBytes)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        mov ebx, dword ptr [ebp + 8]
        mov edi, dword ptr [ebp + 0ch]
        or ebx, ebx
        je lzo_finish_copy
        xor ecx, ecx
        jmp lzo_read_token

lzo_copy_literals:
        lea esi, [ebx + 2]
        rep movsb
        mov ebx, esi
        mov ch, dl
        mov cl, dh
        and edx, 1ch
        shr ch, 5
        shr edx, 2
        neg ecx
        lea esi, [edi + ecx - 1]
        lea ecx, [edx + 3]
        rep movsb
        or cl, byte ptr [ebx]
        mov edx, dword ptr [ebx]
        js lzo_large_token

lzo_match_tail:
        and ecx, 3
        jne lzo_copy_literals
        lea ebx, [ebx + 2]
        mov ch, dl
        mov cl, dh
        and edx, 1ch
        shr ch, 5
        shr edx, 2
        neg ecx
        lea esi, [edi + ecx - 1]
        lea ecx, [edx + 3]
        rep movsb

lzo_read_token:
        or cl, byte ptr [ebx]
        mov edx, dword ptr [ebx]
        jns lzo_match_tail

lzo_large_token:
        add cl, cl
        js lzo_literal_token
        mov cl, dh
        lea esi, [ebx + 3]
        shr ecx, 6
        and ecx, 3
        rep movsb
        mov ebx, esi
        mov ecx, edx
        shr ecx, 10h
        mov ch, dh
        and ch, 3fh
        neg ecx
        lea esi, [edi + ecx - 1]
        and edx, 3fh
        lea ecx, [edx + 4]
        rep movsb
        or cl, byte ptr [ebx]
        mov edx, dword ptr [ebx]
        jns lzo_match_tail
        jmp lzo_large_token

lzo_literal_token:
        add cl, cl
        jns lzo_long_match
        cmp dl, 0fch
        jae lzo_final_literals
        and edx, 1fh
        lea esi, [ebx + 1]
        lea ecx, [edx + 1]
        rep movsd
        mov ebx, esi
        or cl, byte ptr [esi]
        mov edx, dword ptr [esi]
        jns lzo_match_tail
        jmp lzo_large_token

lzo_final_literals:
        mov ecx, edx
        lea esi, [ebx + 1]
        and ecx, 3
        rep movsb

lzo_finish_copy:
        jmp lzo_finish_forward

lzo_long_match:
        mov ecx, edx
        lea esi, [ebx + 4]
        and ecx, 3
        rep movsb
        mov ebx, esi
        mov ecx, edx
        mov eax, edx
        and ecx, 10h
        shr eax, 8
        shl ecx, 0ch
        mov cl, ah
        mov ch, al
        neg ecx
        lea esi, [edi + ecx - 1]
        rol edx, 8
        shr dh, 2
        and edx, 3ffh
        cmp ecx, -4
        jge lzo_short_long_match
        lea ecx, [edx + 5]
        lea edx, [edx + 5]
        shr ecx, 2
        and edx, 3
        rep movsd
        mov ecx, edx
        rep movsb
        or cl, byte ptr [ebx]
        mov edx, dword ptr [ebx]
        jns lzo_match_tail
        jmp lzo_large_token

lzo_short_long_match:
        lea ecx, [edx + 5]
        rep movsb
        or cl, byte ptr [ebx]
        mov edx, dword ptr [ebx]
        jns lzo_match_tail
        jmp lzo_large_token

lzo_finish_forward:
        jmp lzo_finish

lzo_finish:
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
}
