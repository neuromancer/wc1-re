    __asm {
ellipse_0043ce80:
        push ebp
ellipse_0043ce81:
        mov ebp, esp
ellipse_0043ce83:
        add esp, -0x54
ellipse_0043ce86:
        push ebx
ellipse_0043ce87:
        push esi
ellipse_0043ce88:
        push edi
ellipse_0043ce89:
        push es
ellipse_0043ce8a:
        cld
ellipse_0043ce8b:
        push ds
ellipse_0043ce8c:
        pop es
ellipse_0043ce8d:
        cmp dword ptr [ebp + 0x14], 0x0
ellipse_0043ce91:
        jz ellipse_0043ce99
ellipse_0043ce93:
        cmp dword ptr [ebp + 0x18], 0x0
ellipse_0043ce97:
        jnz ellipse_0043ceca
ellipse_0043ce99:
        mov eax, dword ptr [ebp + 0x10]
ellipse_0043ce9c:
        add eax, dword ptr [ebp + 0x18]
ellipse_0043ce9f:
        mov ebx, dword ptr [ebp + 0xc]
ellipse_0043cea2:
        add ebx, dword ptr [ebp + 0x14]
ellipse_0043cea5:
        mov ecx, dword ptr [ebp + 0x10]
ellipse_0043cea8:
        sub ecx, dword ptr [ebp + 0x18]
ellipse_0043ceab:
        mov edx, dword ptr [ebp + 0xc]
ellipse_0043ceae:
        sub edx, dword ptr [ebp + 0x14]
ellipse_0043ceb1:
        push dword ptr [ebp + 0x1c]
ellipse_0043ceb4:
        push 0x0
ellipse_0043ceb6:
        push eax
ellipse_0043ceb7:
        push ebx
ellipse_0043ceb8:
        push ecx
ellipse_0043ceb9:
        push edx
ellipse_0043ceba:
        push dword ptr [ebp + 0x8]
ellipse_0043cebd:
        call DrawClippedLine
ellipse_0043cec2:
        add esp, 0x1c
ellipse_0043cec5:
        jmp ellipse_0043d1bb
ellipse_0043ceca:
        mov esi, dword ptr [ebp + 0x8]
ellipse_0043cecd:
        mov ebx, dword ptr [esi]
ellipse_0043cecf:
        mov eax, dword ptr [ebx + 0x4]
ellipse_0043ced2:
        inc eax
ellipse_0043ced3:
        mov dword ptr [ebp - 0x4c], eax
ellipse_0043ced6:
        jle ellipse_0043cf3c
ellipse_0043ced8:
        mov eax, dword ptr [ebx + 0x8]
ellipse_0043cedb:
        inc eax
ellipse_0043cedc:
        mov ecx, eax
ellipse_0043cede:
        jle ellipse_0043cf3c
ellipse_0043cee0:
        mov eax, dword ptr [esi + 0x4]
ellipse_0043cee3:
        mov dword ptr [ebp - 0x50], eax
ellipse_0043cee6:
        cmp eax, 0x0
ellipse_0043cee9:
        jg ellipse_0043cef0
ellipse_0043ceeb:
        mov eax, 0x0
ellipse_0043cef0:
        mov dword ptr [ebp - 0x38], eax
ellipse_0043cef3:
        mov eax, dword ptr [esi + 0x8]
ellipse_0043cef6:
        mov dword ptr [ebp - 0x54], eax
ellipse_0043cef9:
        cmp eax, 0x0
ellipse_0043cefc:
        jg ellipse_0043cf03
ellipse_0043cefe:
        mov eax, 0x0
ellipse_0043cf03:
        mov dword ptr [ebp - 0x3c], eax
ellipse_0043cf06:
        mov eax, dword ptr [esi + 0xc]
ellipse_0043cf09:
        mov edx, dword ptr [ebp - 0x4c]
ellipse_0043cf0c:
        dec edx
ellipse_0043cf0d:
        cmp eax, edx
ellipse_0043cf0f:
        jl ellipse_0043cf13
ellipse_0043cf11:
        mov eax, edx
ellipse_0043cf13:
        mov dword ptr [ebp - 0x40], eax
ellipse_0043cf16:
        mov eax, dword ptr [esi + 0x10]
ellipse_0043cf19:
        mov edx, ecx
ellipse_0043cf1b:
        dec edx
ellipse_0043cf1c:
        cmp eax, edx
ellipse_0043cf1e:
        jl ellipse_0043cf22
ellipse_0043cf20:
        mov eax, edx
ellipse_0043cf22:
        mov dword ptr [ebp - 0x44], eax
ellipse_0043cf25:
        mov eax, dword ptr [ebp - 0x40]
ellipse_0043cf28:
        cmp eax, dword ptr [ebp - 0x38]
ellipse_0043cf2b:
        jl ellipse_0043cf47
ellipse_0043cf2d:
        mov eax, dword ptr [ebp - 0x44]
ellipse_0043cf30:
        cmp eax, dword ptr [ebp - 0x3c]
ellipse_0043cf33:
        jl ellipse_0043cf47
ellipse_0043cf35:
        mov eax, dword ptr [ebx]
ellipse_0043cf37:
        mov dword ptr [ebp - 0x48], eax
ellipse_0043cf3a:
        jmp ellipse_0043cf52
ellipse_0043cf3c:
        mov eax, 0xffffffff
ellipse_0043cf41:
        pop es
ellipse_0043cf42:
        pop edi
ellipse_0043cf43:
        pop esi
ellipse_0043cf44:
        pop ebx
ellipse_0043cf45:
        leave
ellipse_0043cf46:
        ret
ellipse_0043cf47:
        mov eax, 0xfffffffe
ellipse_0043cf4c:
        pop es
ellipse_0043cf4d:
        pop edi
ellipse_0043cf4e:
        pop esi
ellipse_0043cf4f:
        pop ebx
ellipse_0043cf50:
        leave
ellipse_0043cf51:
        ret
ellipse_0043cf52:
        mov eax, dword ptr [ebp + 0x1c]
ellipse_0043cf55:
        mov ah, al
ellipse_0043cf57:
        mov dword ptr [ebp + 0x1c], eax
ellipse_0043cf5a:
        mov word ptr [ebp + 0x1e], ax
ellipse_0043cf5e:
        mov eax, dword ptr [ebp - 0x50]
ellipse_0043cf61:
        add dword ptr [ebp + 0xc], eax
ellipse_0043cf64:
        mov eax, dword ptr [ebp - 0x54]
ellipse_0043cf67:
        add dword ptr [ebp + 0x10], eax
ellipse_0043cf6a:
        mov eax, dword ptr [ebp + 0xc]
ellipse_0043cf6d:
        mov dword ptr [ebp - 0x4], eax
ellipse_0043cf70:
        mov eax, dword ptr [ebp + 0x10]
ellipse_0043cf73:
        mov dword ptr [ebp - 0x8], eax
ellipse_0043cf76:
        mov dword ptr [ebp - 0xc], 0x0
ellipse_0043cf7d:
        mov eax, dword ptr [ebp + 0x18]
ellipse_0043cf80:
        mov dword ptr [ebp - 0x10], eax
ellipse_0043cf83:
        mul eax
ellipse_0043cf85:
        mov dword ptr [ebp - 0x1c], eax
ellipse_0043cf88:
        shl eax, 0x1
ellipse_0043cf8a:
        mov dword ptr [ebp - 0x20], eax
ellipse_0043cf8d:
        mov eax, dword ptr [ebp + 0x14]
ellipse_0043cf90:
        mul eax
ellipse_0043cf92:
        mov dword ptr [ebp - 0x14], eax
ellipse_0043cf95:
        shl eax, 0x1
ellipse_0043cf97:
        mov dword ptr [ebp - 0x18], eax
ellipse_0043cf9a:
        mov dword ptr [ebp - 0x24], 0x0
ellipse_0043cfa1:
        mov eax, dword ptr [ebp - 0x18]
ellipse_0043cfa4:
        mul dword ptr [ebp + 0x18]
ellipse_0043cfa7:
        mov dword ptr [ebp - 0x28], eax
ellipse_0043cfaa:
        mov eax, dword ptr [ebp - 0x14]
ellipse_0043cfad:
        shr eax, 0x2
ellipse_0043cfb0:
        add eax, dword ptr [ebp - 0x1c]
ellipse_0043cfb3:
        mov dword ptr [ebp - 0x2c], eax
ellipse_0043cfb6:
        mov eax, dword ptr [ebp - 0x14]
ellipse_0043cfb9:
        mul dword ptr [ebp + 0x18]
ellipse_0043cfbc:
        sub dword ptr [ebp - 0x2c], eax
ellipse_0043cfbf:
        mov ebx, dword ptr [ebp + 0x18]
ellipse_0043cfc2:
        mov eax, dword ptr [ebp - 0x24]
ellipse_0043cfc5:
        sub eax, dword ptr [ebp - 0x28]
ellipse_0043cfc8:
        jns ellipse_0043d0b8
ellipse_0043cfce:
        push ebx
ellipse_0043cfcf:
        mov ecx, dword ptr [ebp + 0x1c]
ellipse_0043cfd2:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043cfd5:
        add edi, dword ptr [ebp - 0xc]
ellipse_0043cfd8:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043cfdb:
        add edx, dword ptr [ebp - 0x10]
ellipse_0043cfde:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043cfe1:
        jl ellipse_0043d000
ellipse_0043cfe3:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043cfe6:
        jg ellipse_0043d000
ellipse_0043cfe8:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043cfeb:
        jl ellipse_0043d000
ellipse_0043cfed:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043cff0:
        jg ellipse_0043d000
ellipse_0043cff2:
        mov eax, edx
ellipse_0043cff4:
        imul dword ptr [ebp - 0x4c]
ellipse_0043cff7:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043cffa:
        add eax, edi
ellipse_0043cffc:
        mov edi, eax
ellipse_0043cffe:
        mov byte ptr [edi], CL
ellipse_0043d000:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043d003:
        add edi, dword ptr [ebp - 0xc]
ellipse_0043d006:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043d009:
        sub edx, dword ptr [ebp - 0x10]
ellipse_0043d00c:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043d00f:
        jl ellipse_0043d02e
ellipse_0043d011:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043d014:
        jg ellipse_0043d02e
ellipse_0043d016:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043d019:
        jl ellipse_0043d02e
ellipse_0043d01b:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043d01e:
        jg ellipse_0043d02e
ellipse_0043d020:
        mov eax, edx
ellipse_0043d022:
        imul dword ptr [ebp - 0x4c]
ellipse_0043d025:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043d028:
        add eax, edi
ellipse_0043d02a:
        mov edi, eax
ellipse_0043d02c:
        mov byte ptr [edi], CL
ellipse_0043d02e:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043d031:
        sub edi, dword ptr [ebp - 0xc]
ellipse_0043d034:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043d037:
        add edx, dword ptr [ebp - 0x10]
ellipse_0043d03a:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043d03d:
        jl ellipse_0043d05c
ellipse_0043d03f:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043d042:
        jg ellipse_0043d05c
ellipse_0043d044:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043d047:
        jl ellipse_0043d05c
ellipse_0043d049:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043d04c:
        jg ellipse_0043d05c
ellipse_0043d04e:
        mov eax, edx
ellipse_0043d050:
        imul dword ptr [ebp - 0x4c]
ellipse_0043d053:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043d056:
        add eax, edi
ellipse_0043d058:
        mov edi, eax
ellipse_0043d05a:
        mov byte ptr [edi], CL
ellipse_0043d05c:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043d05f:
        sub edi, dword ptr [ebp - 0xc]
ellipse_0043d062:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043d065:
        sub edx, dword ptr [ebp - 0x10]
ellipse_0043d068:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043d06b:
        jl ellipse_0043d08a
ellipse_0043d06d:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043d070:
        jg ellipse_0043d08a
ellipse_0043d072:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043d075:
        jl ellipse_0043d08a
ellipse_0043d077:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043d07a:
        jg ellipse_0043d08a
ellipse_0043d07c:
        mov eax, edx
ellipse_0043d07e:
        imul dword ptr [ebp - 0x4c]
ellipse_0043d081:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043d084:
        add eax, edi
ellipse_0043d086:
        mov edi, eax
ellipse_0043d088:
        mov byte ptr [edi], CL
ellipse_0043d08a:
        pop ebx
ellipse_0043d08b:
        cmp dword ptr [ebp - 0x2c], 0x0
ellipse_0043d08f:
        js ellipse_0043d0a1
ellipse_0043d091:
        dec dword ptr [ebp - 0x10]
ellipse_0043d094:
        dec ebx
ellipse_0043d095:
        mov eax, dword ptr [ebp - 0x28]
ellipse_0043d098:
        sub eax, dword ptr [ebp - 0x18]
ellipse_0043d09b:
        mov dword ptr [ebp - 0x28], eax
ellipse_0043d09e:
        sub dword ptr [ebp - 0x2c], eax
ellipse_0043d0a1:
        inc dword ptr [ebp - 0xc]
ellipse_0043d0a4:
        mov eax, dword ptr [ebp - 0x24]
ellipse_0043d0a7:
        add eax, dword ptr [ebp - 0x20]
ellipse_0043d0aa:
        mov dword ptr [ebp - 0x24], eax
ellipse_0043d0ad:
        add eax, dword ptr [ebp - 0x1c]
ellipse_0043d0b0:
        add dword ptr [ebp - 0x2c], eax
ellipse_0043d0b3:
        jmp ellipse_0043cfc2
ellipse_0043d0b8:
        mov eax, dword ptr [ebp - 0x14]
ellipse_0043d0bb:
        sub eax, dword ptr [ebp - 0x1c]
ellipse_0043d0be:
        mov edx, eax
ellipse_0043d0c0:
        sar eax, 0x1
ellipse_0043d0c2:
        add eax, edx
ellipse_0043d0c4:
        sub eax, dword ptr [ebp - 0x24]
ellipse_0043d0c7:
        sub eax, dword ptr [ebp - 0x28]
ellipse_0043d0ca:
        sar eax, 0x1
ellipse_0043d0cc:
        add dword ptr [ebp - 0x2c], eax
ellipse_0043d0cf:
        push ebx
ellipse_0043d0d0:
        mov ecx, dword ptr [ebp + 0x1c]
ellipse_0043d0d3:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043d0d6:
        add edi, dword ptr [ebp - 0xc]
ellipse_0043d0d9:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043d0dc:
        add edx, dword ptr [ebp - 0x10]
ellipse_0043d0df:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043d0e2:
        jl ellipse_0043d101
ellipse_0043d0e4:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043d0e7:
        jg ellipse_0043d101
ellipse_0043d0e9:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043d0ec:
        jl ellipse_0043d101
ellipse_0043d0ee:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043d0f1:
        jg ellipse_0043d101
ellipse_0043d0f3:
        mov eax, edx
ellipse_0043d0f5:
        imul dword ptr [ebp - 0x4c]
ellipse_0043d0f8:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043d0fb:
        add eax, edi
ellipse_0043d0fd:
        mov edi, eax
ellipse_0043d0ff:
        mov byte ptr [edi], CL
ellipse_0043d101:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043d104:
        add edi, dword ptr [ebp - 0xc]
ellipse_0043d107:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043d10a:
        sub edx, dword ptr [ebp - 0x10]
ellipse_0043d10d:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043d110:
        jl ellipse_0043d12f
ellipse_0043d112:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043d115:
        jg ellipse_0043d12f
ellipse_0043d117:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043d11a:
        jl ellipse_0043d12f
ellipse_0043d11c:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043d11f:
        jg ellipse_0043d12f
ellipse_0043d121:
        mov eax, edx
ellipse_0043d123:
        imul dword ptr [ebp - 0x4c]
ellipse_0043d126:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043d129:
        add eax, edi
ellipse_0043d12b:
        mov edi, eax
ellipse_0043d12d:
        mov byte ptr [edi], CL
ellipse_0043d12f:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043d132:
        sub edi, dword ptr [ebp - 0xc]
ellipse_0043d135:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043d138:
        add edx, dword ptr [ebp - 0x10]
ellipse_0043d13b:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043d13e:
        jl ellipse_0043d15d
ellipse_0043d140:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043d143:
        jg ellipse_0043d15d
ellipse_0043d145:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043d148:
        jl ellipse_0043d15d
ellipse_0043d14a:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043d14d:
        jg ellipse_0043d15d
ellipse_0043d14f:
        mov eax, edx
ellipse_0043d151:
        imul dword ptr [ebp - 0x4c]
ellipse_0043d154:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043d157:
        add eax, edi
ellipse_0043d159:
        mov edi, eax
ellipse_0043d15b:
        mov byte ptr [edi], CL
ellipse_0043d15d:
        mov edi, dword ptr [ebp - 0x4]
ellipse_0043d160:
        sub edi, dword ptr [ebp - 0xc]
ellipse_0043d163:
        mov edx, dword ptr [ebp - 0x8]
ellipse_0043d166:
        sub edx, dword ptr [ebp - 0x10]
ellipse_0043d169:
        cmp edi, dword ptr [ebp - 0x38]
ellipse_0043d16c:
        jl ellipse_0043d18b
ellipse_0043d16e:
        cmp edi, dword ptr [ebp - 0x40]
ellipse_0043d171:
        jg ellipse_0043d18b
ellipse_0043d173:
        cmp edx, dword ptr [ebp - 0x3c]
ellipse_0043d176:
        jl ellipse_0043d18b
ellipse_0043d178:
        cmp edx, dword ptr [ebp - 0x44]
ellipse_0043d17b:
        jg ellipse_0043d18b
ellipse_0043d17d:
        mov eax, edx
ellipse_0043d17f:
        imul dword ptr [ebp - 0x4c]
ellipse_0043d182:
        add eax, dword ptr [ebp - 0x48]
ellipse_0043d185:
        add eax, edi
ellipse_0043d187:
        mov edi, eax
ellipse_0043d189:
        mov byte ptr [edi], CL
ellipse_0043d18b:
        pop ebx
ellipse_0043d18c:
        cmp dword ptr [ebp - 0x2c], 0x0
ellipse_0043d190:
        jns ellipse_0043d1a1
ellipse_0043d192:
        inc dword ptr [ebp - 0xc]
ellipse_0043d195:
        mov eax, dword ptr [ebp - 0x24]
ellipse_0043d198:
        add eax, dword ptr [ebp - 0x20]
ellipse_0043d19b:
        mov dword ptr [ebp - 0x24], eax
ellipse_0043d19e:
        add dword ptr [ebp - 0x2c], eax
ellipse_0043d1a1:
        dec dword ptr [ebp - 0x10]
ellipse_0043d1a4:
        mov eax, dword ptr [ebp - 0x28]
ellipse_0043d1a7:
        sub eax, dword ptr [ebp - 0x18]
ellipse_0043d1aa:
        mov dword ptr [ebp - 0x28], eax
ellipse_0043d1ad:
        sub eax, dword ptr [ebp - 0x14]
ellipse_0043d1b0:
        sub dword ptr [ebp - 0x2c], eax
ellipse_0043d1b3:
        dec ebx
ellipse_0043d1b4:
        js ellipse_0043d1bb
ellipse_0043d1b6:
        jmp ellipse_0043d0cf
ellipse_0043d1bb:
        pop es
ellipse_0043d1bc:
        pop edi
ellipse_0043d1bd:
        pop esi
ellipse_0043d1be:
        pop ebx
ellipse_0043d1bf:
        leave
ellipse_0043d1c0:
        ret
    }
