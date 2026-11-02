; @file
; @ingroup graphics
; @brief Hand-written raster library: RLE sprites, clipped lines, fills
; and ellipses, and IFF, PCX and GIF image decoding.
;
; Assembled rather than compiled: its shifts by one use the D1/D0
; encodings and it keeps tables in its code segment. It was linked
; between screens.obj and killbrd.obj: code 439c40-43f635, data
; 46e69c-47008b. FillRasterBytes, just before it, is compiled C at the
; end of src/screens.c.
;
; Extracted from the retail executable by bin/extractRasterLibrary.py,
; which also checks that UASM reproduces its bytes and relocations.
; Regenerate rather than edit: the script holds the names and comments.

        .386
        .model  flat, c
        option  casemap:none

        public  CopyRasterDriverName
        public  InstallRasterDriverCallbacks
        public  SetRasterClipPixel
        public  ReadRasterClipPixel
        public  DrawClippedLine
        public  FillRasterClipCheckerboard
        public  DrawRLEImage
        public  DrawRLEImageUnclipped
        public  SetPaletteTranslationTable
        public  DrawRLEImageColor
        public  DrawRLEImageColorUnclipped
        public  RotateRLEImage
        public  GetRLEFrameBounds
        public  EncodeRasterClipToRLEFrame
        public  TranslateRLEFramePalette
        public  EncodeRLEScanline
        public  EmitRLEScanlineRun
        public  FillRasterClip
        public  BlitRasterClip
        public  ScrollRasterClipWrapped
        public  DrawRasterEllipse
        public  FillRasterEllipse
        public  GetRLETransformTrig
        public  CalculateRoundedRLEFixedProduct
        public  TransformRLEPoint
        public  GetRawImageHeight
        public  GetRawFrameWidth
        public  BlitRawFrame
        public  BlitSelectedRawFrames
        public  BlitRawScanline
        public  FindIFFChunkData
        public  DecodeIFFImage
        public  CopyILBMPalette
        public  GetILBMImageSize
        public  DecodePCXImage
        public  CopyPCXPaletteFromFileTail
        public  GetPCXImageSize
        public  ResetGIFLZWDictionary
        public  ReadGIFDataSubBlockByte
        public  ReadGIFLZWCode
        public  AppendGIFLZWDictionaryEntry
        public  EmitGIFDecodedPixel
        public  ExpandGIFLZWImage
        public  CopyGIFPalette
        public  GetGIFImageSize
        public  GetRLEFrameDimensions
        public  GetRLEFrameExtents
        public  GetRLEImageSize
        public  GetRLEImageOrigin
        public  ApplyRLEFramePalette
        public  CopyRLEFramePalette
        public  SetRLEFramePalette
        public  GetRLEFrameCount
        public  CollectUniqueRLEImageFrames
        public  CollectUniqueRLEPaletteFrames
        public  DissolveRasterClip
        public  FadeRasterPaletteToPalette
        public  CollectRasterClipColours
        public  anRLEQuarterCosine
        public  abIffBmhdChunkId
        public  abIffCmapChunkId
        public  abIffBodyChunkId
        public  apRasterDriverCallbacks
        public  szRasterDriverName
        public  pRLEEncodeBuffer
        public  nRLEPendingSkip
        public  pRLEScanlineStart
        public  pRLEScanCursor
        public  pRLEOutputCursor
        public  pRLELiteralStart
        public  nRLEEncodedMinimumX
        public  nRLEEncodedMinimumY
        public  nRLEEncodedMaximumX
        public  nRLEEncodedMaximumY
        public  abRasterDecodeBuffer
        public  abRasterPaletteUsageScratch
        public  abRasterFadeAccumulator
        public  abGifLZWCodeMasks
        public  abGifInterlaceRowSteps
        public  abGifInterlaceRowStarts
        public  pGifRasterTarget
        public  abRasterPaletteTranslation
        public  aRLETransformVertices
        public  anRLESourceSteps

        .code

; Function start: 0x00439C40
; char *CopyRasterDriverName(void *const *callbacks);
; Unreferenced. Copies the raster driver name out of its callback table.
CopyRasterDriverName proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        call dword ptr [esi]
        mov edi, offset szRasterDriverName
L439C54:
        mov bl, byte ptr [eax]
        mov byte ptr [edi], bl
        inc edi
        inc eax
        or bl, bl
        jne L439C54
        mov eax, offset szRasterDriverName
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CopyRasterDriverName endp

; Function start: 0x00439C69
; void InstallRasterDriverCallbacks(void *const *callbacks);
; Unreferenced. Copies thirteen driver callbacks into the library.
InstallRasterDriverCallbacks proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov edi, offset apRasterDriverCallbacks
        mov ecx, 0Dh
        rep movsd
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
InstallRasterDriverCallbacks endp

; Function start: 0x00439C88
; unsigned int SetRasterClipPixel(RasterClip *clip, int x, int y, int colour);
; Writes one pixel through a clip, with explicit segment setup.
SetRasterClipPixel proc
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-18h], eax
        jle L439D07
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L439D07
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-1Ch], eax
        cmp eax, 0
        jg L439CBB
        mov eax, 0
L439CBB:
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-20h], eax
        cmp eax, 0
        jg L439CCE
        mov eax, 0
L439CCE:
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-18h]
        dec edx
        cmp eax, edx
        jl L439CDE
        mov eax, edx
L439CDE:
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L439CED
        mov eax, edx
L439CED:
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [ebp-0Ch]
        cmp eax, dword ptr [ebp-4]
        jl L439D12
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [ebp-8]
        jl L439D12
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-14h], eax
        jmp L439D1D
L439D07:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439D12:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439D1D:
        mov ecx, dword ptr [ebp+0Ch]
        mov ebx, dword ptr [ebp+10h]
        add ecx, dword ptr [ebp-1Ch]
        add ebx, dword ptr [ebp-20h]
        cmp ecx, dword ptr [ebp-4]
        jl L439D58
        cmp ecx, dword ptr [ebp-0Ch]
        jg L439D58
        cmp ebx, dword ptr [ebp-8]
        jl L439D58
        cmp ebx, dword ptr [ebp-10h]
        jg L439D58
        mov eax, ebx
        imul dword ptr [ebp-18h]
        add eax, dword ptr [ebp-14h]
        add eax, ecx
        mov ebx, eax
        xor eax, eax
        mov al, byte ptr [ebx]
        mov dl, byte ptr [ebp+14h]
        mov byte ptr [ebx], dl
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439D58:
        mov eax, 0FFFFFFFDh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
SetRasterClipPixel endp

; Function start: 0x00439D63
; int ReadRasterClipPixel(RasterClip *clip, int x, int y);
; Reads one pixel through a clip.
ReadRasterClipPixel proc
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-18h], eax
        jle L439DE2
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L439DE2
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-1Ch], eax
        cmp eax, 0
        jg L439D96
        mov eax, 0
L439D96:
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-20h], eax
        cmp eax, 0
        jg L439DA9
        mov eax, 0
L439DA9:
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-18h]
        dec edx
        cmp eax, edx
        jl L439DB9
        mov eax, edx
L439DB9:
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L439DC8
        mov eax, edx
L439DC8:
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [ebp-0Ch]
        cmp eax, dword ptr [ebp-4]
        jl L439DED
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [ebp-8]
        jl L439DED
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-14h], eax
        jmp L439DF8
L439DE2:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439DED:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439DF8:
        mov ecx, dword ptr [ebp+0Ch]
        mov ebx, dword ptr [ebp+10h]
        add ecx, dword ptr [ebp-1Ch]
        add ebx, dword ptr [ebp-20h]
        cmp ecx, dword ptr [ebp-4]
        jl L439E2E
        cmp ecx, dword ptr [ebp-0Ch]
        jg L439E2E
        cmp ebx, dword ptr [ebp-8]
        jl L439E2E
        cmp ebx, dword ptr [ebp-10h]
        jg L439E2E
        mov eax, ebx
        imul dword ptr [ebp-18h]
        add eax, dword ptr [ebp-14h]
        add eax, ecx
        mov ebx, eax
        xor eax, eax
        mov al, byte ptr [ebx]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439E2E:
        mov eax, 0FFFFFFFDh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
ReadRasterClipPixel endp

; Function start: 0x00439E39
; unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1, int x2, int
;                              y2, int mode, int colour);
; Draws a clipped line: mode 0 writes a colour, mode 1 translates through
; a colour table, and higher modes call the callback passed as the colour.
DrawClippedLine proc
        push ebp
        mov ebp, esp
        add esp, -58h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-50h], eax
        jle L439EB8
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L439EB8
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-54h], eax
        cmp eax, 0
        jg L439E6C
        mov eax, 0
L439E6C:
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-58h], eax
        cmp eax, 0
        jg L439E7F
        mov eax, 0
L439E7F:
        mov dword ptr [ebp-40h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-50h]
        dec edx
        cmp eax, edx
        jl L439E8F
        mov eax, edx
L439E8F:
        mov dword ptr [ebp-44h], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L439E9E
        mov eax, edx
L439E9E:
        mov dword ptr [ebp-48h], eax
        mov eax, dword ptr [ebp-44h]
        cmp eax, dword ptr [ebp-3Ch]
        jl L439EC3
        mov eax, dword ptr [ebp-48h]
        cmp eax, dword ptr [ebp-40h]
        jl L439EC3
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-4Ch], eax
        jmp L439ECE
L439EB8:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439EC3:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L439ECE:
        mov eax, dword ptr [ebp-54h]
        add dword ptr [ebp+0Ch], eax
        add dword ptr [ebp+14h], eax
        mov eax, dword ptr [ebp-58h]
        add dword ptr [ebp+10h], eax
        add dword ptr [ebp+18h], eax
        mov eax, dword ptr [ebp+14h]
        sub eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-4], eax
        cdq
        mov dword ptr [ebp-0Ch], edx
        xor eax, edx
        sub eax, edx
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [ebp+18h]
        sub eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-10h], eax
        cdq
        mov dword ptr [ebp-18h], edx
        xor eax, edx
        sub eax, edx
        mov dword ptr [ebp-14h], eax
        mov eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-24h], eax
        mov eax, dword ptr [ebp+14h]
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-28h], eax
        mov eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-30h], eax
        cmp dword ptr [ebp-4], 0
        je L43A64F
        cmp dword ptr [ebp-10h], 0
        je L43A6DA
        mov eax, dword ptr [ebp-0Ch]
        xor eax, dword ptr [ebp-18h]
        mov dword ptr [ebp-1Ch], eax
        mov edx, dword ptr [ebp-8]
        mov ebx, dword ptr [ebp-14h]
        mov eax, 0FFFFFFFFh
        cmp edx, ebx
        je L439F54
        jl L439F50
        xchg edx, ebx
L439F50:
        xor eax, eax
        div ebx
L439F54:
        mov dword ptr [ebp-20h], eax
        mov dword ptr [ebp-34h], 0
L439F5E:
        xor edx, edx
        mov eax, dword ptr [ebp-24h]
        sub eax, dword ptr [ebp-3Ch]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-44h]
        sub eax, dword ptr [ebp-24h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [ebp-40h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-48h]
        sub eax, dword ptr [ebp-28h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-2Ch]
        sub eax, dword ptr [ebp-3Ch]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-44h]
        sub eax, dword ptr [ebp-2Ch]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-30h]
        sub eax, dword ptr [ebp-40h]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-48h]
        sub eax, dword ptr [ebp-30h]
        shl eax, 1
        adc dh, dh
        or dword ptr [ebp-34h], edx
        or edx, edx
        je L43A31D
        test dl, dh
        jne L43A830
        mov ebx, dword ptr [ebp-8]
        cmp ebx, dword ptr [ebp-14h]
        jl L43A018
        test dl, 8
        jne L43A061
        test dl, 4
        jne L43A0B5
        test dl, 2
        jne L43A139
        test dl, 1
        jne L43A191
        test dh, 8
        jne L43A1C1
        test dh, 4
        jne L43A21C
        test dh, 2
        jne L43A29B
        test dh, 1
        jne L43A2F2
        jmp L439F5E
L43A018:
        test dl, 8
        jne L43A089
        test dl, 4
        jne L43A0E1
        test dl, 2
        jne L43A111
        test dl, 1
        jne L43A165
        test dh, 8
        jne L43A1ED
        test dh, 4
        jne L43A244
        test dh, 2
        jne L43A26F
        test dh, 1
        jne L43A2CA
        jmp L439F5E
L43A061:
        mov eax, dword ptr [ebp-3Ch]
        mov dword ptr [ebp-24h], eax
        sub eax, dword ptr [ebp+0Ch]
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-28h], eax
        jmp L439F5E
L43A089:
        mov eax, dword ptr [ebp-3Ch]
        mov dword ptr [ebp-24h], eax
        sub eax, dword ptr [ebp+0Ch]
        mov edx, eax
        dec edx
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, -1
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-28h], eax
        jmp L439F5E
L43A0B5:
        mov eax, dword ptr [ebp-44h]
        mov dword ptr [ebp-24h], eax
        sub eax, dword ptr [ebp+0Ch]
        neg eax
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-28h], eax
        jmp L439F5E
L43A0E1:
        mov eax, dword ptr [ebp-44h]
        mov dword ptr [ebp-24h], eax
        sub eax, dword ptr [ebp+0Ch]
        neg eax
        mov edx, eax
        dec edx
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, -1
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-28h], eax
        jmp L439F5E
L43A111:
        mov eax, dword ptr [ebp-40h]
        mov dword ptr [ebp-28h], eax
        sub eax, dword ptr [ebp+10h]
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-24h], eax
        jmp L439F5E
L43A139:
        mov eax, dword ptr [ebp-40h]
        mov dword ptr [ebp-28h], eax
        sub eax, dword ptr [ebp+10h]
        mov edx, eax
        dec edx
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, -1
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-24h], eax
        jmp L439F5E
L43A165:
        mov eax, dword ptr [ebp-48h]
        mov dword ptr [ebp-28h], eax
        sub eax, dword ptr [ebp+10h]
        neg eax
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-24h], eax
        jmp L439F5E
L43A191:
        mov eax, dword ptr [ebp-48h]
        mov dword ptr [ebp-28h], eax
        sub eax, dword ptr [ebp+10h]
        neg eax
        mov edx, eax
        dec edx
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, -1
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-24h], eax
        jmp L439F5E
L43A1C1:
        mov eax, dword ptr [ebp-3Ch]
        mov dword ptr [ebp-2Ch], eax
        sub eax, dword ptr [ebp+0Ch]
        neg eax
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-30h], eax
        jmp L439F5E
L43A1ED:
        mov eax, dword ptr [ebp-3Ch]
        mov dword ptr [ebp-2Ch], eax
        sub eax, dword ptr [ebp+0Ch]
        neg eax
        mov edx, eax
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, 0
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-30h], eax
        jmp L439F5E
L43A21C:
        mov eax, dword ptr [ebp-44h]
        mov dword ptr [ebp-2Ch], eax
        sub eax, dword ptr [ebp+0Ch]
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-30h], eax
        jmp L439F5E
L43A244:
        mov eax, dword ptr [ebp-44h]
        mov dword ptr [ebp-2Ch], eax
        sub eax, dword ptr [ebp+0Ch]
        mov edx, eax
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, 0
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-30h], eax
        jmp L439F5E
L43A26F:
        mov eax, dword ptr [ebp-40h]
        mov dword ptr [ebp-30h], eax
        sub eax, dword ptr [ebp+10h]
        neg eax
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-2Ch], eax
        jmp L439F5E
L43A29B:
        mov eax, dword ptr [ebp-40h]
        mov dword ptr [ebp-30h], eax
        sub eax, dword ptr [ebp+10h]
        neg eax
        mov edx, eax
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, 0
        mov edx, dword ptr [ebp-1Ch]
        not edx
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-2Ch], eax
        jmp L439F5E
L43A2CA:
        mov eax, dword ptr [ebp-48h]
        mov dword ptr [ebp-30h], eax
        sub eax, dword ptr [ebp+10h]
        mul dword ptr [ebp-20h]
        add eax, 80000000h
        adc edx, 0
        mov eax, edx
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-2Ch], eax
        jmp L439F5E
L43A2F2:
        mov eax, dword ptr [ebp-48h]
        mov dword ptr [ebp-30h], eax
        sub eax, dword ptr [ebp+10h]
        mov edx, eax
        mov eax, 80000000h
        div dword ptr [ebp-20h]
        cmp edx, 1
        sbb eax, 0
        mov edx, dword ptr [ebp-1Ch]
        xor eax, edx
        sub eax, edx
        add eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-2Ch], eax
        jmp L439F5E
L43A31D:
        mov eax, dword ptr [ebp-28h]
        imul dword ptr [ebp-50h]
        add eax, dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-24h]
        mov edi, eax
        mov esi, dword ptr [ebp-50h]
        xor esi, dword ptr [ebp-18h]
        sub esi, dword ptr [ebp-18h]
        mov ebx, dword ptr [ebp-20h]
        mov eax, dword ptr [ebp-8]
        cmp eax, dword ptr [ebp-14h]
        je L43A760
        jg L43A4B1
        mov eax, dword ptr [ebp-30h]
        sub eax, dword ptr [ebp-28h]
        cdq
        xor eax, edx
        sub eax, edx
        inc eax
        mov ecx, eax
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [ebp+10h]
        cdq
        xor eax, edx
        sub eax, edx
        mul ebx
        add eax, 80000000h
        mov edx, eax
        cmp dword ptr [ebp-0Ch], -1
        je L43A412
        cmp dword ptr [ebp+1Ch], 1
        je L43A3A9
        jg L43A3E7
        mov eax, dword ptr [ebp+20h]
L43A380:
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        je L43A3A4
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        je L43A3A4
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        je L43A3A4
        mov byte ptr [edi], al
        add edx, ebx
        adc edi, esi
        dec ecx
        jne L43A380
L43A3A4:
        jmp L43A821
L43A3A9:
        mov eax, dword ptr [ebp+20h]
        push ebp
        mov ebp, ebx
        mov ebx, eax
L43A3B1:
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        je L43A3E1
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        je L43A3E1
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        je L43A3E1
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        adc edi, esi
        dec ecx
        jne L43A3B1
L43A3E1:
        pop ebp
        jmp L43A821
L43A3E7:
        mov esi, dword ptr [ebp+8]
        mov edi, dword ptr [ebp-24h]
        sub edi, dword ptr [esi+4]
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [esi+8]
        mov esi, eax
        mov eax, dword ptr [ebp-18h]
        add eax, eax
        inc eax
L43A3FE:
        pushad
        call dword ptr [ebp+20h]
        popad
        add edx, ebx
        jae L43A408
        inc edi
L43A408:
        add esi, eax
        dec ecx
        jne L43A3FE
        jmp L43A821
L43A412:
        neg esi
        cmp dword ptr [ebp+1Ch], 1
        je L43A448
        jg L43A486
        mov eax, dword ptr [ebp+20h]
L43A41F:
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        je L43A443
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        je L43A443
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        je L43A443
        mov byte ptr [edi], al
        add edx, ebx
        sbb edi, esi
        dec ecx
        jne L43A41F
L43A443:
        jmp L43A821
L43A448:
        mov eax, dword ptr [ebp+20h]
        push ebp
        mov ebp, ebx
        mov ebx, eax
L43A450:
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        je L43A480
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        je L43A480
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        je L43A480
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edx, ebp
        sbb edi, esi
        dec ecx
        jne L43A450
L43A480:
        pop ebp
        jmp L43A821
L43A486:
        mov esi, dword ptr [ebp+8]
        mov edi, dword ptr [ebp-24h]
        sub edi, dword ptr [esi+4]
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [esi+8]
        mov esi, eax
        mov eax, dword ptr [ebp-18h]
        add eax, eax
        inc eax
L43A49D:
        pushad
        call dword ptr [ebp+20h]
        popad
        add edx, ebx
        jae L43A4A7
        dec edi
L43A4A7:
        add esi, eax
        dec ecx
        jne L43A49D
        jmp L43A821
L43A4B1:
        mov eax, dword ptr [ebp-2Ch]
        sub eax, dword ptr [ebp-24h]
        cdq
        xor eax, edx
        sub eax, edx
        inc eax
        mov ecx, eax
        mov eax, dword ptr [ebp-24h]
        sub eax, dword ptr [ebp+0Ch]
        cdq
        xor eax, edx
        sub eax, edx
        mul ebx
        add eax, 80000000h
        mov edx, eax
        cmp dword ptr [ebp-0Ch], -1
        je L43A596
        cmp dword ptr [ebp+1Ch], 1
        je L43A521
        jg L43A56B
        mov eax, dword ptr [ebp+20h]
L43A4EC:
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae L43A4F5
        add edi, esi
L43A4F5:
        dec ecx
        je L43A51C
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae L43A501
        add edi, esi
L43A501:
        dec ecx
        je L43A51C
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae L43A50D
        add edi, esi
L43A50D:
        dec ecx
        je L43A51C
        mov byte ptr [edi], al
        inc edi
        add edx, ebx
        jae L43A519
        add edi, esi
L43A519:
        dec ecx
        jne L43A4EC
L43A51C:
        jmp L43A821
L43A521:
        mov eax, dword ptr [ebp+20h]
        push ebp
        mov ebp, ebx
        mov ebx, eax
L43A529:
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae L43A535
        add edi, esi
L43A535:
        dec ecx
        je L43A565
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae L43A544
        add edi, esi
L43A544:
        dec ecx
        je L43A565
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae L43A553
        add edi, esi
L43A553:
        dec ecx
        je L43A565
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        inc edi
        add edx, ebp
        jae L43A562
        add edi, esi
L43A562:
        dec ecx
        jne L43A529
L43A565:
        pop ebp
        jmp L43A821
L43A56B:
        mov esi, dword ptr [ebp+8]
        mov edi, dword ptr [ebp-24h]
        sub edi, dword ptr [esi+4]
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [esi+8]
        mov esi, eax
        mov eax, dword ptr [ebp-18h]
        add eax, eax
        inc eax
L43A582:
        pushad
        call dword ptr [ebp+20h]
        popad
        add edx, ebx
        jae L43A58C
        inc esi
L43A58C:
        add edi, eax
        dec ecx
        jne L43A582
        jmp L43A821
L43A596:
        cmp dword ptr [ebp+1Ch], 1
        je L43A5DA
        jg L43A624
        mov eax, dword ptr [ebp+20h]
L43A5A5:
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae L43A5AE
        add edi, esi
L43A5AE:
        dec ecx
        je L43A5D5
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae L43A5BA
        add edi, esi
L43A5BA:
        dec ecx
        je L43A5D5
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae L43A5C6
        add edi, esi
L43A5C6:
        dec ecx
        je L43A5D5
        mov byte ptr [edi], al
        dec edi
        add edx, ebx
        jae L43A5D2
        add edi, esi
L43A5D2:
        dec ecx
        jne L43A5A5
L43A5D5:
        jmp L43A821
L43A5DA:
        mov eax, dword ptr [ebp+20h]
        push ebp
        mov ebp, ebx
        mov ebx, eax
L43A5E2:
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae L43A5EE
        add edi, esi
L43A5EE:
        dec ecx
        je L43A61E
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae L43A5FD
        add edi, esi
L43A5FD:
        dec ecx
        je L43A61E
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae L43A60C
        add edi, esi
L43A60C:
        dec ecx
        je L43A61E
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        dec edi
        add edx, ebp
        jae L43A61B
        add edi, esi
L43A61B:
        dec ecx
        jne L43A5E2
L43A61E:
        pop ebp
        jmp L43A821
L43A624:
        mov esi, dword ptr [ebp+8]
        mov edi, dword ptr [ebp-24h]
        sub edi, dword ptr [esi+4]
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [esi+8]
        mov esi, eax
        mov eax, dword ptr [ebp-18h]
        add eax, eax
        inc eax
L43A63B:
        pushad
        call dword ptr [ebp+20h]
        popad
        add edx, ebx
        jae L43A645
        dec esi
L43A645:
        add edi, eax
        dec ecx
        jne L43A63B
        jmp L43A821
L43A64F:
        mov eax, dword ptr [ebp+0Ch]
        cmp eax, dword ptr [ebp-3Ch]
        jl L43A830
        cmp eax, dword ptr [ebp-44h]
        jg L43A830
        mov eax, dword ptr [ebp+10h]
        cmp eax, dword ptr [ebp+18h]
        jg L43A66F
        mov eax, dword ptr [ebp+18h]
L43A66F:
        cmp eax, dword ptr [ebp-40h]
        jl L43A830
        mov eax, dword ptr [ebp+10h]
        cmp eax, dword ptr [ebp+18h]
        jl L43A683
        mov eax, dword ptr [ebp+18h]
L43A683:
        cmp eax, dword ptr [ebp-48h]
        jg L43A830
        mov eax, dword ptr [ebp+10h]
        cmp eax, dword ptr [ebp-40h]
        jg L43A697
        mov eax, dword ptr [ebp-40h]
L43A697:
        cmp eax, dword ptr [ebp-48h]
        jl L43A69F
        mov eax, dword ptr [ebp-48h]
L43A69F:
        mov dword ptr [ebp-28h], eax
        mov eax, dword ptr [ebp+18h]
        cmp eax, dword ptr [ebp-40h]
        jg L43A6AD
        mov eax, dword ptr [ebp-40h]
L43A6AD:
        cmp eax, dword ptr [ebp-48h]
        jl L43A6B5
        mov eax, dword ptr [ebp-48h]
L43A6B5:
        mov dword ptr [ebp-30h], eax
        mov eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-24h], eax
        mov esi, dword ptr [ebp-50h]
        xor esi, dword ptr [ebp-18h]
        sub esi, dword ptr [ebp-18h]
        mov eax, dword ptr [ebp-30h]
        sub eax, dword ptr [ebp-28h]
        cdq
        xor eax, edx
        sub eax, edx
        mov ecx, eax
        inc ecx
        jmp L43A780
L43A6DA:
        mov eax, dword ptr [ebp+10h]
        cmp eax, dword ptr [ebp-40h]
        jl L43A830
        cmp eax, dword ptr [ebp-48h]
        jg L43A830
        mov eax, dword ptr [ebp+0Ch]
        cmp eax, dword ptr [ebp+14h]
        jg L43A6FA
        mov eax, dword ptr [ebp+14h]
L43A6FA:
        cmp eax, dword ptr [ebp-3Ch]
        jl L43A830
        mov eax, dword ptr [ebp+0Ch]
        cmp eax, dword ptr [ebp+14h]
        jl L43A70E
        mov eax, dword ptr [ebp+14h]
L43A70E:
        cmp eax, dword ptr [ebp-44h]
        jg L43A830
        mov eax, dword ptr [ebp+0Ch]
        cmp eax, dword ptr [ebp-3Ch]
        jg L43A722
        mov eax, dword ptr [ebp-3Ch]
L43A722:
        cmp eax, dword ptr [ebp-44h]
        jl L43A72A
        mov eax, dword ptr [ebp-44h]
L43A72A:
        mov dword ptr [ebp-24h], eax
        mov eax, dword ptr [ebp+14h]
        cmp eax, dword ptr [ebp-3Ch]
        jg L43A738
        mov eax, dword ptr [ebp-3Ch]
L43A738:
        cmp eax, dword ptr [ebp-44h]
        jl L43A740
        mov eax, dword ptr [ebp-44h]
L43A740:
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-28h], eax
        mov esi, dword ptr [ebp-0Ch]
        inc esi
        or esi, dword ptr [ebp-0Ch]
        mov eax, dword ptr [ebp-2Ch]
        sub eax, dword ptr [ebp-24h]
        cdq
        xor eax, edx
        sub eax, edx
        mov ecx, eax
        inc ecx
        jmp L43A780
L43A760:
        mov esi, dword ptr [ebp-50h]
        xor esi, dword ptr [ebp-18h]
        sub esi, dword ptr [ebp-18h]
        mov eax, dword ptr [ebp-0Ch]
        inc eax
        or eax, dword ptr [ebp-0Ch]
        add esi, eax
        mov eax, dword ptr [ebp-2Ch]
        sub eax, dword ptr [ebp-24h]
        cdq
        xor eax, edx
        sub eax, edx
        mov ecx, eax
        inc ecx
L43A780:
        mov eax, dword ptr [ebp-28h]
        imul dword ptr [ebp-50h]
        add eax, dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-24h]
        mov edi, eax
        cmp dword ptr [ebp+1Ch], 1
        je L43A7B7
        jg L43A7E4
        mov eax, dword ptr [ebp+20h]
L43A799:
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je L43A7B5
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je L43A7B5
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je L43A7B5
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        jne L43A799
L43A7B5:
        jmp L43A821
L43A7B7:
        mov ebx, dword ptr [ebp+20h]
L43A7BA:
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je L43A7E2
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je L43A7E2
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        je L43A7E2
        mov al, byte ptr [edi]
        xlatb
        mov byte ptr [edi], al
        add edi, esi
        dec ecx
        jne L43A7BA
L43A7E2:
        jmp L43A821
L43A7E4:
        mov esi, dword ptr [ebp+8]
        mov edi, dword ptr [ebp-24h]
        sub edi, dword ptr [esi+4]
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [esi+8]
        mov esi, eax
        xor eax, eax
        test dword ptr [ebp-10h], 0FFFFFFFFh
        setne al
        or eax, dword ptr [ebp-18h]
        xor ebx, ebx
        test dword ptr [ebp-4], 0FFFFFFFFh
        setne bl
        or ebx, dword ptr [ebp-0Ch]
L43A813:
        pushad
        call dword ptr [ebp+20h]
        popad
        add esi, eax
        add edi, ebx
        dec ecx
        jne L43A813
        jmp L43A821
L43A821:
        xor eax, eax
        cmp dword ptr [ebp-34h], 1
        setae al
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43A830:
        mov eax, 2
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DrawClippedLine endp

; Function start: 0x0043A83B
; int FillRasterClipCheckerboard(RasterClip *clip, int left, int top, int
;                                right, int bottom, unsigned char colour);
; Fills a clip with a two-colour checkerboard.
FillRasterClipCheckerboard proc
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-18h], eax
        jle L43A8BA
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43A8BA
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-1Ch], eax
        cmp eax, 0
        jg L43A86E
        mov eax, 0
L43A86E:
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-20h], eax
        cmp eax, 0
        jg L43A881
        mov eax, 0
L43A881:
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-18h]
        dec edx
        cmp eax, edx
        jl L43A891
        mov eax, edx
L43A891:
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43A8A0
        mov eax, edx
L43A8A0:
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [ebp-0Ch]
        cmp eax, dword ptr [ebp-4]
        jl L43A8C5
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [ebp-8]
        jl L43A8C5
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-14h], eax
        jmp L43A8D0
L43A8BA:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43A8C5:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43A8D0:
        mov eax, dword ptr [ebp-1Ch]
        add dword ptr [ebp+0Ch], eax
        add dword ptr [ebp+14h], eax
        mov eax, dword ptr [ebp-20h]
        add dword ptr [ebp+10h], eax
        add dword ptr [ebp+18h], eax
        mov eax, dword ptr [ebp-4]
        cmp dword ptr [ebp+0Ch], eax
        jg L43A8ED
        mov dword ptr [ebp+0Ch], eax
L43A8ED:
        mov eax, dword ptr [ebp-8]
        cmp dword ptr [ebp+10h], eax
        jg L43A8F8
        mov dword ptr [ebp+10h], eax
L43A8F8:
        mov eax, dword ptr [ebp-0Ch]
        cmp dword ptr [ebp+14h], eax
        jl L43A903
        mov dword ptr [ebp+14h], eax
L43A903:
        mov eax, dword ptr [ebp-10h]
        cmp dword ptr [ebp+18h], eax
        jl L43A90E
        mov dword ptr [ebp+18h], eax
L43A90E:
        mov ecx, dword ptr [ebp+14h]
        sub ecx, dword ptr [ebp+0Ch]
        jl L43A969
        inc ecx
        mov eax, dword ptr [ebp+10h]
        imul dword ptr [ebp-18h]
        add eax, dword ptr [ebp-14h]
        add eax, dword ptr [ebp+0Ch]
        mov edi, eax
        mov edx, dword ptr [ebp+18h]
        sub edx, dword ptr [ebp+10h]
        jl L43A969
        mov eax, dword ptr [ebp+1Ch]
        mov esi, edi
        mov ebx, ecx
        jmp L43A93D
L43A936:
        add esi, dword ptr [ebp-18h]
        mov edi, esi
        mov ecx, ebx
L43A93D:
        push edx
        and edx, 1
        je L43A948
        pop edx
        inc edi
        dec ecx
        jmp L43A949
L43A948:
        pop edx
L43A949:
        mov byte ptr [edi], al
        add edi, 2
        sub ecx, 2
        jg L43A949
        dec edx
        jns L43A936
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
        mov eax, 0FFFFFFFDh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43A969:
        mov eax, 0FFFFFFFCh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
FillRasterClipCheckerboard endp

; Function start: 0x0043A974
; int DrawRLEImage(RasterClip *clip, unsigned char *shape, int frame, int x,
;                  int y);
; Draws a clipped RLE frame.
DrawRLEImage proc
        push ebp
        mov ebp, esp
        add esp, -50h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-48h], eax
        jle L43A9F3
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43A9F3
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-4Ch], eax
        cmp eax, 0
        jg L43A9A7
        mov eax, 0
L43A9A7:
        mov dword ptr [ebp-34h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-50h], eax
        cmp eax, 0
        jg L43A9BA
        mov eax, 0
L43A9BA:
        mov dword ptr [ebp-38h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-48h]
        dec edx
        cmp eax, edx
        jl L43A9CA
        mov eax, edx
L43A9CA:
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43A9D9
        mov eax, edx
L43A9D9:
        mov dword ptr [ebp-40h], eax
        mov eax, dword ptr [ebp-3Ch]
        cmp eax, dword ptr [ebp-34h]
        jl L43A9FE
        mov eax, dword ptr [ebp-40h]
        cmp eax, dword ptr [ebp-38h]
        jl L43A9FE
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-44h], eax
        jmp L43AA09
L43A9F3:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43A9FE:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43AA09:
        mov eax, dword ptr [ebp-4Ch]
        add dword ptr [ebp+14h], eax
        mov eax, dword ptr [ebp-50h]
        add dword ptr [ebp+18h], eax
        mov esi, dword ptr [ebp+10h]
        shl esi, 3
        add esi, 8
        add esi, dword ptr [ebp+0Ch]
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-30h], esi
        mov eax, dword ptr [esi+8]
        add eax, dword ptr [ebp+14h]
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+0Ch]
        add eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [esi+10h]
        add eax, dword ptr [ebp+14h]
        mov dword ptr [ebp-14h], eax
        mov eax, dword ptr [esi+14h]
        add eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-18h], eax
        add esi, 18h
        mov eax, dword ptr [ebp-14h]
        cmp eax, dword ptr [ebp-0Ch]
        jl L43AD6D
        mov eax, dword ptr [ebp-18h]
        cmp eax, dword ptr [ebp-10h]
        jl L43AD6D
        xor edx, edx
        mov eax, dword ptr [ebp-0Ch]
        sub eax, dword ptr [ebp-34h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-3Ch]
        sub eax, dword ptr [ebp-0Ch]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-10h]
        sub eax, dword ptr [ebp-38h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-40h]
        sub eax, dword ptr [ebp-10h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-14h]
        sub eax, dword ptr [ebp-34h]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-3Ch]
        sub eax, dword ptr [ebp-14h]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-18h]
        sub eax, dword ptr [ebp-38h]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-40h]
        sub eax, dword ptr [ebp-18h]
        shl eax, 1
        adc dh, dh
        mov dword ptr [ebp-1Ch], edx
        test dl, dh
        jne L43AD62
        or dl, dh
        jne L43AAF4
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+4]
        sub dword ptr [ebp+14h], eax
        mov eax, dword ptr [esi+8]
        sub dword ptr [ebp+18h], eax
        push dword ptr [ebp-48h]
        push dword ptr [ebp+18h]
        push dword ptr [ebp+14h]
        push dword ptr [ebp-30h]
        push dword ptr [ebp+8]
        call DrawRLEImageUnclipped
        add esp, 14h
        jmp L43AD5A
L43AAF4:
        mov eax, dword ptr [ebp-10h]
        imul dword ptr [ebp-48h]
        add eax, dword ptr [ebp-44h]
        add eax, dword ptr [ebp-0Ch]
        mov edi, eax
        mov ecx, dword ptr [ebp-10h]
        mov dword ptr [ebp-20h], ecx
        jmp L43AB20
L43AB0A:
        movzx eax, al
        add esi, eax
        dec esi
L43AB10:
        inc esi
L43AB11:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AB10
        jne L43AB0A
        jb L43AB10
        add edi, dword ptr [ebp-48h]
        inc ecx
L43AB20:
        cmp ecx, dword ptr [ebp-38h]
        jl L43AB11
        mov dword ptr [ebp-24h], edi
        mov dword ptr [ebp-20h], ecx
        mov eax, edi
        sub eax, dword ptr [ebp-0Ch]
        add eax, dword ptr [ebp-34h]
        mov dword ptr [ebp-28h], eax
        mov eax, edi
        sub eax, dword ptr [ebp-0Ch]
        add eax, dword ptr [ebp-3Ch]
        mov dword ptr [ebp-2Ch], eax
        jmp L43AD4E
L43AB46:
        mov eax, dword ptr [ebp-20h]
        cmp eax, dword ptr [ebp-40h]
        jg L43AD5A
        mov edi, dword ptr [ebp-24h]
        test dword ptr [ebp-1Ch], 8
        jne L43ABD3
        test dword ptr [ebp-1Ch], 400h
        jne L43AC5D
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AB8B
        jne L43ABB4
        jae L43ABCE
L43AB76:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AB8B
        jne L43ABB4
        jb L43AB76
        jae L43ABCE
L43AB8B:
        movzx ecx, al
L43AB8E:
        mov al, byte ptr [esi]
        inc esi
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AB8B
        jae L43ABCE
        je L43AB76
L43ABB4:
        movzx ecx, al
L43ABB7:
        push ecx
        and ecx, 3
        rep movsb
        pop ecx
        shr ecx, 2
        rep movsd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AB8B
        jne L43ABB4
        jb L43AB76
L43ABCE:
        jmp L43AD3F
L43ABD3:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ABF3
        jne L43AC24
        jae L43AC58
L43ABDE:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ABF3
        jne L43AC24
        jb L43ABDE
        jae L43AC58
L43ABF3:
        movzx ecx, al
        mov eax, dword ptr [ebp-28h]
        sub eax, edi
        cmp eax, ecx
        jge L43AC16
        or eax, eax
        js L43AC07
        add edi, eax
        sub ecx, eax
L43AC07:
        test dword ptr [ebp-1Ch], 400h
        je L43AB8E
        jne L43AC84
L43AC16:
        add edi, ecx
        inc esi
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ABF3
        jae L43AC58
        je L43ABDE
L43AC24:
        movzx ecx, al
        mov eax, dword ptr [ebp-28h]
        sub eax, edi
        cmp eax, ecx
        jge L43AC49
        or eax, eax
        js L43AC3A
        add edi, eax
        sub ecx, eax
        add esi, eax
L43AC3A:
        test dword ptr [ebp-1Ch], 400h
        je L43ABB7
        jne L43ACC7
L43AC49:
        add edi, ecx
        add esi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ABF3
        jne L43AC24
        jb L43ABDE
L43AC58:
        jmp L43AD3F
L43AC5D:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AC81
        jne L43ACC4
        jae L43ACFA
L43AC6C:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AC81
        jne L43ACC4
        jb L43AC6C
        jae L43ACFA
L43AC81:
        movzx ecx, al
L43AC84:
        cmp edi, dword ptr [ebp-2Ch]
        jg L43AD1F
        mov eax, edi
        add eax, ecx
        dec eax
        sub eax, dword ptr [ebp-2Ch]
        cdq
        not edx
        and edx, eax
        sub ecx, edx
        mov al, byte ptr [esi]
        inc esi
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        add edi, edx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AC81
        jae L43ACFA
        je L43AC6C
L43ACC4:
        movzx ecx, al
L43ACC7:
        cmp edi, dword ptr [ebp-2Ch]
        jg L43AD30
        mov eax, edi
        add eax, ecx
        dec eax
        sub eax, dword ptr [ebp-2Ch]
        cdq
        not edx
        and edx, eax
        sub ecx, edx
        push ecx
        and ecx, 3
        rep movsb
        pop ecx
        shr ecx, 2
        rep movsd
        add edi, edx
        add esi, edx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AC81
        jne L43ACC4
        jb L43AC6C
L43ACFA:
        jmp L43AD3F
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AD1C
        jne L43AD2D
        jae L43AD3F
L43AD07:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AD1C
        jne L43AD2D
        jb L43AD07
        jae L43AD3F
L43AD1C:
        movzx ecx, al
L43AD1F:
        add edi, ecx
        inc esi
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AD1C
        jae L43AD3F
        je L43AD07
L43AD2D:
        movzx ecx, al
L43AD30:
        add edi, ecx
        add esi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AD1C
        jne L43AD2D
        jb L43AD07
L43AD3F:
        mov eax, dword ptr [ebp-48h]
        add dword ptr [ebp-24h], eax
        add dword ptr [ebp-28h], eax
        add dword ptr [ebp-2Ch], eax
        inc dword ptr [ebp-20h]
L43AD4E:
        mov eax, dword ptr [ebp-20h]
        cmp eax, dword ptr [ebp-18h]
        jle L43AB46
L43AD5A:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43AD62:
        mov eax, 0FFFFFFFDh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43AD6D:
        mov eax, 0FFFFFFFCh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DrawRLEImage endp

; Function start: 0x0043AD78
; int DrawRLEImageUnclipped(RasterClip *clip, RLEFrameHeader *frameHeader, int
;                           x, int y, int strideScratch);
; Draws an RLE frame that lies entirely inside the clip.
DrawRLEImageUnclipped proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi+4]
        add dword ptr [ebp+10h], eax
        mov eax, dword ptr [esi+8]
        add dword ptr [ebp+14h], eax
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp+18h], eax
        jle L43AE37
        mov esi, dword ptr [ebp+0Ch]
        mov edi, dword ptr [ebx]
        mov eax, dword ptr [esi+8]
        add eax, dword ptr [ebp+10h]
        add edi, eax
        mov eax, dword ptr [esi+0Ch]
        mov ebx, eax
        add eax, dword ptr [ebp+14h]
        mul dword ptr [ebp+18h]
        add edi, eax
        mov edx, edi
        mov eax, dword ptr [esi+10h]
        mov eax, dword ptr [esi+14h]
        inc eax
        sub eax, ebx
        mov ebx, eax
        jle L43AE37
        add esi, 18h
L43ADCC:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ADEC
        jne L43AE15
        jae L43AE2F
L43ADD7:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ADEC
        jne L43AE15
        jb L43ADD7
        jae L43AE2F
L43ADEC:
        movzx ecx, al
        mov al, byte ptr [esi]
        inc esi
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ADEC
        jae L43AE2F
        je L43ADD7
L43AE15:
        movzx ecx, al
        push ecx
        and ecx, 3
        rep movsb
        pop ecx
        shr ecx, 2
        rep movsd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43ADEC
        jne L43AE15
        jb L43ADD7
L43AE2F:
        add edx, dword ptr [ebp+18h]
        mov edi, edx
        dec ebx
        jne L43ADCC
L43AE37:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DrawRLEImageUnclipped endp

; Function start: 0x0043AE3F
; void SetPaletteTranslationTable(const unsigned char *translation);
; Selects the colour translation table for the colour RLE blitters.
SetPaletteTranslationTable proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov edi, offset abRasterPaletteTranslation
        mov ecx, 40h
        rep movsd
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
SetPaletteTranslationTable endp

; Function start: 0x0043AE5E
; int DrawRLEImageColor(RasterClip *clip, unsigned char *shape, int frame, int
;                       x, int y);
; Draws a clipped RLE frame through the colour translation table.
DrawRLEImageColor proc
        push ebp
        mov ebp, esp
        add esp, -50h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-48h], eax
        jle L43AEDD
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43AEDD
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-4Ch], eax
        cmp eax, 0
        jg L43AE91
        mov eax, 0
L43AE91:
        mov dword ptr [ebp-34h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-50h], eax
        cmp eax, 0
        jg L43AEA4
        mov eax, 0
L43AEA4:
        mov dword ptr [ebp-38h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-48h]
        dec edx
        cmp eax, edx
        jl L43AEB4
        mov eax, edx
L43AEB4:
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43AEC3
        mov eax, edx
L43AEC3:
        mov dword ptr [ebp-40h], eax
        mov eax, dword ptr [ebp-3Ch]
        cmp eax, dword ptr [ebp-34h]
        jl L43AEE8
        mov eax, dword ptr [ebp-40h]
        cmp eax, dword ptr [ebp-38h]
        jl L43AEE8
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-44h], eax
        jmp L43AEF3
L43AEDD:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43AEE8:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43AEF3:
        mov eax, dword ptr [ebp-4Ch]
        add dword ptr [ebp+14h], eax
        mov eax, dword ptr [ebp-50h]
        add dword ptr [ebp+18h], eax
        mov esi, dword ptr [ebp+10h]
        shl esi, 3
        add esi, 8
        add esi, dword ptr [ebp+0Ch]
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-30h], esi
        mov eax, dword ptr [esi+8]
        add eax, dword ptr [ebp+14h]
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+0Ch]
        add eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [esi+10h]
        add eax, dword ptr [ebp+14h]
        mov dword ptr [ebp-14h], eax
        mov eax, dword ptr [esi+14h]
        add eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-18h], eax
        add esi, 18h
        mov eax, dword ptr [ebp-14h]
        cmp eax, dword ptr [ebp-0Ch]
        jl L43B32B
        mov eax, dword ptr [ebp-18h]
        cmp eax, dword ptr [ebp-10h]
        jl L43B32B
        xor edx, edx
        mov eax, dword ptr [ebp-0Ch]
        sub eax, dword ptr [ebp-34h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-3Ch]
        sub eax, dword ptr [ebp-0Ch]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-10h]
        sub eax, dword ptr [ebp-38h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-40h]
        sub eax, dword ptr [ebp-10h]
        shl eax, 1
        adc dl, dl
        mov eax, dword ptr [ebp-14h]
        sub eax, dword ptr [ebp-34h]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-3Ch]
        sub eax, dword ptr [ebp-14h]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-18h]
        sub eax, dword ptr [ebp-38h]
        shl eax, 1
        adc dh, dh
        mov eax, dword ptr [ebp-40h]
        sub eax, dword ptr [ebp-18h]
        shl eax, 1
        adc dh, dh
        mov dword ptr [ebp-1Ch], edx
        test dl, dh
        jne L43B320
        or dl, dh
        jne L43AFDE
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+4]
        sub dword ptr [ebp+14h], eax
        mov eax, dword ptr [esi+8]
        sub dword ptr [ebp+18h], eax
        push dword ptr [ebp-48h]
        push dword ptr [ebp+18h]
        push dword ptr [ebp+14h]
        push dword ptr [ebp-30h]
        push dword ptr [ebp+8]
        call DrawRLEImageColorUnclipped
        add esp, 14h
        jmp L43B318
L43AFDE:
        mov eax, dword ptr [ebp-10h]
        imul dword ptr [ebp-48h]
        add eax, dword ptr [ebp-44h]
        add eax, dword ptr [ebp-0Ch]
        mov edi, eax
        mov ecx, dword ptr [ebp-10h]
        mov dword ptr [ebp-20h], ecx
        jmp L43B00A
L43AFF4:
        movzx eax, al
        add esi, eax
        dec esi
L43AFFA:
        inc esi
L43AFFB:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43AFFA
        jne L43AFF4
        jb L43AFFA
        add edi, dword ptr [ebp-48h]
        inc ecx
L43B00A:
        cmp ecx, dword ptr [ebp-38h]
        jl L43AFFB
        mov dword ptr [ebp-24h], edi
        mov dword ptr [ebp-20h], ecx
        mov eax, edi
        sub eax, dword ptr [ebp-0Ch]
        add eax, dword ptr [ebp-34h]
        mov dword ptr [ebp-28h], eax
        mov eax, edi
        sub eax, dword ptr [ebp-0Ch]
        add eax, dword ptr [ebp-3Ch]
        mov dword ptr [ebp-2Ch], eax
        jmp L43B30C
L43B030:
        mov eax, dword ptr [ebp-20h]
        cmp eax, dword ptr [ebp-40h]
        jg L43B318
        mov edi, dword ptr [ebp-24h]
        test dword ptr [ebp-1Ch], 8
        jne L43B125
        test dword ptr [ebp-1Ch], 400h
        jne L43B1B3
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B081
        jne L43B0B2
        jae L43B120
L43B068:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B081
        jne L43B0B2
        jb L43B068
        jae L43B120
L43B081:
        movzx ecx, al
L43B084:
        xor eax, eax
        mov al, byte ptr [esi]
        inc esi
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B081
        jae L43B120
        je L43B068
L43B0B2:
        movzx ecx, al
L43B0B5:
        xor eax, eax
        or ecx, ecx
        je L43B10D
        cmp ecx, 4
        jl L43B0FE
L43B0C0:
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi], al
        mov al, byte ptr [esi+1]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+1], al
        mov al, byte ptr [esi+2]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+2], al
        mov al, byte ptr [esi+3]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+3], al
        add esi, 4
        add edi, 4
        sub ecx, 4
        je L43B10D
        cmp ecx, 4
        jge L43B0C0
L43B0FE:
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi], al
        inc esi
        inc edi
        dec ecx
        jne L43B0FE
L43B10D:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B081
        jne L43B0B2
        jb L43B068
L43B120:
        jmp L43B2FD
L43B125:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B145
        jne L43B176
        jae L43B1AE
L43B130:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B145
        jne L43B176
        jb L43B130
        jae L43B1AE
L43B145:
        movzx ecx, al
        mov eax, dword ptr [ebp-28h]
        sub eax, edi
        cmp eax, ecx
        jge L43B168
        or eax, eax
        js L43B159
        add edi, eax
        sub ecx, eax
L43B159:
        test dword ptr [ebp-1Ch], 400h
        je L43B084
        jne L43B1DE
L43B168:
        add edi, ecx
        inc esi
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B145
        jae L43B1AE
        je L43B130
L43B176:
        movzx ecx, al
        mov eax, dword ptr [ebp-28h]
        sub eax, edi
        cmp eax, ecx
        jge L43B19F
        or eax, eax
        js L43B18C
        add edi, eax
        sub ecx, eax
        add esi, eax
L43B18C:
        test dword ptr [ebp-1Ch], 400h
        je L43B0B5
        jne L43B22D
L43B19F:
        add edi, ecx
        add esi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B145
        jne L43B176
        jb L43B130
L43B1AE:
        jmp L43B2FD
L43B1B3:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B1DB
        jne L43B22A
        jae L43B2B8
L43B1C2:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B1DB
        jne L43B22A
        jb L43B1C2
        jae L43B2B8
L43B1DB:
        movzx ecx, al
L43B1DE:
        cmp edi, dword ptr [ebp-2Ch]
        jg L43B2DD
        mov eax, edi
        add eax, ecx
        dec eax
        sub eax, dword ptr [ebp-2Ch]
        cdq
        not edx
        and edx, eax
        sub ecx, edx
        xor eax, eax
        mov al, byte ptr [esi]
        inc esi
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        add edi, edx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B1DB
        jae L43B2B8
        je L43B1C2
L43B22A:
        movzx ecx, al
L43B22D:
        cmp edi, dword ptr [ebp-2Ch]
        jg L43B2EE
        mov eax, edi
        add eax, ecx
        dec eax
        sub eax, dword ptr [ebp-2Ch]
        cdq
        not edx
        and edx, eax
        sub ecx, edx
        xor eax, eax
        or ecx, ecx
        je L43B29D
        cmp ecx, 4
        jl L43B28E
L43B250:
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi], al
        mov al, byte ptr [esi+1]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+1], al
        mov al, byte ptr [esi+2]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+2], al
        mov al, byte ptr [esi+3]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+3], al
        add esi, 4
        add edi, 4
        sub ecx, 4
        je L43B29D
        cmp ecx, 4
        jge L43B250
L43B28E:
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi], al
        inc esi
        inc edi
        dec ecx
        jne L43B28E
L43B29D:
        add edi, edx
        add esi, edx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B1DB
        jne L43B22A
        jb L43B1C2
L43B2B8:
        jmp L43B2FD
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B2DA
        jne L43B2EB
        jae L43B2FD
L43B2C5:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B2DA
        jne L43B2EB
        jb L43B2C5
        jae L43B2FD
L43B2DA:
        movzx ecx, al
L43B2DD:
        add edi, ecx
        inc esi
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B2DA
        jae L43B2FD
        je L43B2C5
L43B2EB:
        movzx ecx, al
L43B2EE:
        add edi, ecx
        add esi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B2DA
        jne L43B2EB
        jb L43B2C5
L43B2FD:
        mov eax, dword ptr [ebp-48h]
        add dword ptr [ebp-24h], eax
        add dword ptr [ebp-28h], eax
        add dword ptr [ebp-2Ch], eax
        inc dword ptr [ebp-20h]
L43B30C:
        mov eax, dword ptr [ebp-20h]
        cmp eax, dword ptr [ebp-18h]
        jle L43B030
L43B318:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43B320:
        mov eax, 0FFFFFFFDh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43B32B:
        mov eax, 0FFFFFFFCh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DrawRLEImageColor endp

; Function start: 0x0043B336
; int DrawRLEImageColorUnclipped(RasterClip *clip, RLEFrameHeader
;                                *frameHeader, int x, int y, int
;                                strideScratch);
; Draws an unclipped RLE frame through the colour translation table.
DrawRLEImageColorUnclipped proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi+4]
        add dword ptr [ebp+10h], eax
        mov eax, dword ptr [esi+8]
        add dword ptr [ebp+14h], eax
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp+18h], eax
        jle L43B461
        mov esi, dword ptr [ebp+0Ch]
        mov edi, dword ptr [ebx]
        mov eax, dword ptr [esi+8]
        add eax, dword ptr [ebp+10h]
        add edi, eax
        mov eax, dword ptr [esi+0Ch]
        mov ebx, eax
        add eax, dword ptr [ebp+14h]
        mul dword ptr [ebp+18h]
        add edi, eax
        mov edx, edi
        mov eax, dword ptr [esi+10h]
        mov eax, dword ptr [esi+14h]
        inc eax
        sub eax, ebx
        mov ebx, eax
        jle L43B461
        add esi, 18h
L43B38E:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B3B6
        jne L43B3E7
        jae L43B455
L43B39D:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B3B6
        jne L43B3E7
        jb L43B39D
        jae L43B455
L43B3B6:
        movzx ecx, al
        xor eax, eax
        mov al, byte ptr [esi]
        inc esi
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B3B6
        jae L43B455
        je L43B39D
L43B3E7:
        movzx ecx, al
        xor eax, eax
        or ecx, ecx
        je L43B442
        cmp ecx, 4
        jl L43B433
L43B3F5:
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi], al
        mov al, byte ptr [esi+1]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+1], al
        mov al, byte ptr [esi+2]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+2], al
        mov al, byte ptr [esi+3]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi+3], al
        add esi, 4
        add edi, 4
        sub ecx, 4
        je L43B442
        cmp ecx, 4
        jge L43B3F5
L43B433:
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [edi], al
        inc esi
        inc edi
        dec ecx
        jne L43B433
L43B442:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43B3B6
        jne L43B3E7
        jb L43B39D
L43B455:
        add edx, dword ptr [ebp+18h]
        mov edi, edx
        dec ebx
        jne L43B38E
L43B461:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DrawRLEImageColorUnclipped endp

; Function start: 0x0043B469
; int RotateRLEImage(RasterClip *clip, unsigned char *shape, int frame, int x,
;                    int y, unsigned char *scratch, unsigned int angleTenths,
;                    int scaleX, int scaleY, unsigned int flags);
; Draws an RLE frame rotated and scaled through the transform vertices.
RotateRLEImage proc
        push ebp
        mov ebp, esp
        add esp, 0FFFFFF10h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        cmp dword ptr [ebp+24h], 10000h
        jne L43B495
        cmp dword ptr [ebp+28h], 10000h
        jne L43B495
        cmp dword ptr [ebp+20h], 0
        je L43BFD3
L43B495:
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        call GetRLEImageSize
        add esp, 8
        mov ecx, eax
        shr eax, 10h
        dec eax
        mov dword ptr [ebp-44h], eax
        and ecx, 0FFFFh
        dec ecx
        mov dword ptr [ebp-48h], ecx
        lea ebx, dword ptr [ebp-14h]
        lea esi, dword ptr [ebp-28h]
        mov dword ptr [esi], ebx
        mov edx, dword ptr [ebp+1Ch]
        mov dword ptr [ebx], edx
        mov dword ptr [esi+4], 0
        mov dword ptr [aRLETransformVertices+0Ch], 0
        mov dword ptr [aRLETransformVertices+48h], 0
        mov dword ptr [esi+8], 0
        mov dword ptr [aRLETransformVertices+10h], 0
        mov dword ptr [aRLETransformVertices+24h], 0
        mov dword ptr [ebx+4], eax
        mov dword ptr [esi+0Ch], eax
        mov dword ptr [aRLETransformVertices+20h], eax
        mov dword ptr [aRLETransformVertices+34h], eax
        mov dword ptr [ebx+8], ecx
        mov dword ptr [esi+10h], ecx
        mov dword ptr [aRLETransformVertices+38h], ecx
        mov dword ptr [aRLETransformVertices+4Ch], ecx
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        call GetRLEImageOrigin
        add esp, 8
        mov ebx, eax
        cwde
        neg eax
        mov dword ptr [ebp-3Ch], eax
        sar ebx, 10h
        neg ebx
        mov dword ptr [ebp-40h], ebx
        test dword ptr [ebp+2Ch], 2
        jne L43B58E
        push 0FFh
        lea eax, dword ptr [ebp-28h]
        push eax
        call FillRasterClip
        add esp, 8
        test dword ptr [ebp+2Ch], 1
        je L43B576
        push dword ptr [ebp-3Ch]
        push dword ptr [ebp-40h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        lea eax, dword ptr [ebp-28h]
        push eax
        call DrawRLEImageColor
        add esp, 14h
        jmp L43B58E
L43B576:
        push dword ptr [ebp-3Ch]
        push dword ptr [ebp-40h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        lea eax, dword ptr [ebp-28h]
        push eax
        call DrawRLEImage
        add esp, 14h
L43B58E:
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-0E8h], eax
        jle L43B62F
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43B62F
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-0ECh], eax
        cmp eax, 0
        jg L43B5C2
        mov eax, 0
L43B5C2:
        mov dword ptr [ebp-0D4h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-0F0h], eax
        cmp eax, 0
        jg L43B5DB
        mov eax, 0
L43B5DB:
        mov dword ptr [ebp-0D8h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-0E8h]
        dec edx
        cmp eax, edx
        jl L43B5F1
        mov eax, edx
L43B5F1:
        mov dword ptr [ebp-0DCh], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43B603
        mov eax, edx
L43B603:
        mov dword ptr [ebp-0E0h], eax
        mov eax, dword ptr [ebp-0DCh]
        cmp eax, dword ptr [ebp-0D4h]
        jl L43B63A
        mov eax, dword ptr [ebp-0E0h]
        cmp eax, dword ptr [ebp-0D8h]
        jl L43B63A
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-0E4h], eax
        jmp L43B645
L43B62F:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43B63A:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43B645:
        mov eax, dword ptr [ebp+14h]
        sub eax, dword ptr [ebp-40h]
        mov dword ptr [ebp-4Ch], eax
        mov eax, dword ptr [ebp+18h]
        sub eax, dword ptr [ebp-3Ch]
        mov dword ptr [ebp-50h], eax
        mov dword ptr [ebp-30h], 0
        mov dword ptr [ebp-2Ch], 0
        push dword ptr [ebp+28h]
        push dword ptr [ebp+24h]
        push dword ptr [ebp+20h]
        lea eax, dword ptr [ebp-40h]
        push eax
        lea eax, dword ptr [ebp-38h]
        push eax
        lea eax, dword ptr [ebp-30h]
        push eax
        call TransformRLEPoint
        add esp, 18h
        mov eax, dword ptr [ebp-38h]
        add eax, dword ptr [ebp-4Ch]
        mov dword ptr [aRLETransformVertices], eax
        mov eax, dword ptr [ebp-34h]
        add eax, dword ptr [ebp-50h]
        mov dword ptr [aRLETransformVertices+4], eax
        mov eax, dword ptr [ebp-44h]
        mov dword ptr [ebp-30h], eax
        mov dword ptr [ebp-2Ch], 0
        push dword ptr [ebp+28h]
        push dword ptr [ebp+24h]
        push dword ptr [ebp+20h]
        lea eax, dword ptr [ebp-40h]
        push eax
        lea eax, dword ptr [ebp-38h]
        push eax
        lea eax, dword ptr [ebp-30h]
        push eax
        call TransformRLEPoint
        add esp, 18h
        mov eax, dword ptr [ebp-38h]
        add eax, dword ptr [ebp-4Ch]
        mov dword ptr [aRLETransformVertices+14h], eax
        mov eax, dword ptr [ebp-34h]
        add eax, dword ptr [ebp-50h]
        mov dword ptr [aRLETransformVertices+18h], eax
        mov eax, dword ptr [ebp-0ECh]
        add dword ptr [aRLETransformVertices], eax
        add dword ptr [aRLETransformVertices+14h], eax
        mov eax, dword ptr [ebp-0F0h]
        add dword ptr [aRLETransformVertices+4], eax
        add dword ptr [aRLETransformVertices+18h], eax
        mov eax, dword ptr [ebp-44h]
        mov ebx, dword ptr [ebp-48h]
        mov dword ptr [ebp-30h], eax
        mov dword ptr [ebp-2Ch], ebx
        push dword ptr [ebp+28h]
        push dword ptr [ebp+24h]
        push dword ptr [ebp+20h]
        lea eax, dword ptr [ebp-40h]
        push eax
        lea eax, dword ptr [ebp-38h]
        push eax
        lea eax, dword ptr [ebp-30h]
        push eax
        call TransformRLEPoint
        add esp, 18h
        mov eax, dword ptr [ebp-38h]
        add eax, dword ptr [ebp-4Ch]
        mov dword ptr [aRLETransformVertices+28h], eax
        mov eax, dword ptr [ebp-34h]
        add eax, dword ptr [ebp-50h]
        mov dword ptr [aRLETransformVertices+2Ch], eax
        mov eax, dword ptr [ebp-48h]
        mov dword ptr [ebp-30h], 0
        mov dword ptr [ebp-2Ch], eax
        push dword ptr [ebp+28h]
        push dword ptr [ebp+24h]
        push dword ptr [ebp+20h]
        lea eax, dword ptr [ebp-40h]
        push eax
        lea eax, dword ptr [ebp-38h]
        push eax
        lea eax, dword ptr [ebp-30h]
        push eax
        call TransformRLEPoint
        add esp, 18h
        mov eax, dword ptr [ebp-38h]
        add eax, dword ptr [ebp-4Ch]
        mov dword ptr [aRLETransformVertices+3Ch], eax
        mov eax, dword ptr [ebp-34h]
        add eax, dword ptr [ebp-50h]
        mov dword ptr [aRLETransformVertices+40h], eax
        mov eax, dword ptr [ebp-0ECh]
        add dword ptr [aRLETransformVertices+28h], eax
        add dword ptr [aRLETransformVertices+3Ch], eax
        mov eax, dword ptr [ebp-0F0h]
        add dword ptr [aRLETransformVertices+2Ch], eax
        add dword ptr [aRLETransformVertices+40h], eax
        lea ebx, dword ptr [ebp-14h]
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-58h], eax
        mov ecx, dword ptr [ebx+4]
        inc ecx
        mov dword ptr [ebp-54h], ecx
        push ds
        pop es
        mov ebx, offset aRLETransformVertices
        mov eax, ebx
        add eax, 50h
        mov dword ptr [ebp-64h], ebx
        mov dword ptr [ebp-68h], eax
        mov esi, 7FFFh
        mov edi, 0FFFF8000h
        mov ecx, 0Fh
L43B7CF:
        mov edx, 0
        mov eax, dword ptr [ebx]
        sub eax, dword ptr [ebp-0D4h]
        shld edx, eax, 1
        mov eax, dword ptr [ebp-0DCh]
        sub eax, dword ptr [ebx]
        shld edx, eax, 1
        mov eax, dword ptr [ebx+4]
        sub eax, dword ptr [ebp-0D8h]
        shld edx, eax, 1
        mov eax, dword ptr [ebp-0E0h]
        sub eax, dword ptr [ebx+4]
        shld edx, eax, 1
        mov eax, dword ptr [ebx+4]
        cmp eax, esi
        jg L43B812
        mov esi, eax
        mov dword ptr [ebp-6Ch], ebx
L43B812:
        cmp eax, edi
        jl L43B818
        mov edi, eax
L43B818:
        and ecx, edx
        add ebx, 14h
        cmp ebx, dword ptr [ebp-68h]
        jne L43B7CF
        or ecx, ecx
        jne L43BE04
        mov eax, dword ptr [ebp-6Ch]
        mov dword ptr [ebp-78h], eax
        mov dword ptr [ebp-7Ch], eax
        mov dword ptr [ebp-8Ch], esi
        cmp edi, esi
        je L43BE04
L43B841:
        mov ebx, dword ptr [ebp-78h]
        mov dword ptr [ebp-70h], ebx
        mov esi, ebx
        sub esi, 14h
        cmp esi, dword ptr [ebp-64h]
        jge L43B857
        mov esi, dword ptr [ebp-68h]
        sub esi, 14h
L43B857:
        mov dword ptr [ebp-78h], esi
        mov ecx, dword ptr [esi+4]
        mov edx, dword ptr [ebx+4]
        cmp edx, dword ptr [ebp-0D8h]
        jge L43B870
        cmp ecx, dword ptr [ebp-0D8h]
        jle L43B841
L43B870:
        sub ecx, edx
        je L43B841
        mov dword ptr [ebp-80h], ecx
        mov edx, dword ptr [esi]
        sub edx, dword ptr [ebx]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0A8h], eax
        mov ecx, dword ptr [ebp-80h]
        mov edx, dword ptr [esi+0Ch]
        sub edx, dword ptr [ebx+0Ch]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0B0h], eax
        mov ecx, dword ptr [ebp-80h]
        mov edx, dword ptr [esi+10h]
        sub edx, dword ptr [ebx+10h]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0B8h], eax
        mov edx, dword ptr [ebx]
        shl edx, 10h
        add edx, 8000h
        mov dword ptr [ebp-90h], edx
        mov edx, dword ptr [ebx+0Ch]
        shl edx, 10h
        add edx, 8000h
        mov dword ptr [ebp-98h], edx
        mov edx, dword ptr [ebx+10h]
        shl edx, 10h
        add edx, 8000h
        mov dword ptr [ebp-0A0h], edx
L43B907:
        mov ebx, dword ptr [ebp-7Ch]
        mov dword ptr [ebp-74h], ebx
        mov esi, ebx
        add esi, 14h
        cmp esi, dword ptr [ebp-68h]
        jl L43B91A
        mov esi, dword ptr [ebp-64h]
L43B91A:
        mov dword ptr [ebp-7Ch], esi
        mov ecx, dword ptr [esi+4]
        mov edx, dword ptr [ebx+4]
        cmp edx, dword ptr [ebp-0D8h]
        jge L43B933
        cmp ecx, dword ptr [ebp-0D8h]
        jle L43B907
L43B933:
        sub ecx, edx
        je L43B907
        mov dword ptr [ebp-84h], ecx
        mov edx, dword ptr [esi]
        sub edx, dword ptr [ebx]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0ACh], eax
        mov ecx, dword ptr [ebp-84h]
        mov edx, dword ptr [esi+0Ch]
        sub edx, dword ptr [ebx+0Ch]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0B4h], eax
        mov ecx, dword ptr [ebp-84h]
        mov edx, dword ptr [esi+10h]
        sub edx, dword ptr [ebx+10h]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0BCh], eax
        mov edx, dword ptr [ebx]
        shl edx, 10h
        add edx, 8000h
        mov dword ptr [ebp-94h], edx
        mov edx, dword ptr [ebx+0Ch]
        shl edx, 10h
        add edx, 8000h
        mov dword ptr [ebp-9Ch], edx
        mov edx, dword ptr [ebx+10h]
        shl edx, 10h
        add edx, 8000h
        mov dword ptr [ebp-0A4h], edx
        mov eax, dword ptr [ebp-0E0h]
        sub eax, dword ptr [ebp-8Ch]
        sub edi, dword ptr [ebp-0E0h]
        jg L43B9E9
        add eax, edi
L43B9E9:
        mov dword ptr [ebp-88h], eax
        mov eax, dword ptr [ebp-0D8h]
        sub eax, dword ptr [ebp-8Ch]
        jle L43BAA0
        sub dword ptr [ebp-88h], eax
        mov ecx, dword ptr [ebp-0D8h]
        mov dword ptr [ebp-8Ch], ecx
        mov ebx, dword ptr [ebp-70h]
        sub ecx, dword ptr [ebx+4]
        sub dword ptr [ebp-80h], ecx
        shl ecx, 10h
        mov eax, dword ptr [ebp-0A8h]
        imul ecx
        shrd eax, edx, 10h
        add dword ptr [ebp-90h], eax
        mov eax, dword ptr [ebp-0B0h]
        imul ecx
        shrd eax, edx, 10h
        add dword ptr [ebp-98h], eax
        mov eax, dword ptr [ebp-0B8h]
        imul ecx
        shrd eax, edx, 10h
        add dword ptr [ebp-0A0h], eax
        mov ecx, dword ptr [ebp-0D8h]
        mov ebx, dword ptr [ebp-74h]
        sub ecx, dword ptr [ebx+4]
        sub dword ptr [ebp-84h], ecx
        shl ecx, 10h
        mov eax, dword ptr [ebp-0ACh]
        imul ecx
        shrd eax, edx, 10h
        add dword ptr [ebp-94h], eax
        mov eax, dword ptr [ebp-0B4h]
        imul ecx
        shrd eax, edx, 10h
        add dword ptr [ebp-9Ch], eax
        mov eax, dword ptr [ebp-0BCh]
        imul ecx
        shrd eax, edx, 10h
        add dword ptr [ebp-0A4h], eax
L43BAA0:
        mov eax, dword ptr [ebp-8Ch]
        imul dword ptr [ebp-0E8h]
        add eax, dword ptr [ebp-0E4h]
        add eax, 0
        mov dword ptr [ebp-5Ch], eax
        mov eax, dword ptr [ebp-90h]
        mov ebx, dword ptr [ebp-94h]
        mov ecx, dword ptr [ebp-98h]
        mov edx, dword ptr [ebp-9Ch]
        mov esi, dword ptr [ebp-0A0h]
        mov edi, dword ptr [ebp-0A4h]
L43BADC:
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        cmp ebx, eax
        jg L43BAEB
        xchg eax, ebx
        xchg ecx, edx
        xchg esi, edi
L43BAEB:
        sar eax, 10h
        cmp eax, dword ptr [ebp-0DCh]
        jg L43BDAD
        sar ebx, 10h
        cmp ebx, dword ptr [ebp-0D4h]
        jl L43BDAD
        mov dword ptr [ebp-0C0h], eax
        mov dword ptr [ebp-0C4h], ebx
        mov dword ptr [ebp-0D0h], ecx
        sub ebx, eax
        je L43BBF0
        push ebx
        sub edx, ecx
        xor eax, eax
        shrd eax, edx, 10h
        shl ebx, 10h
        sar edx, 10h
        idiv ebx
        mov dword ptr [ebp-0C8h], eax
        shld edx, eax, 10h
        pop ebx
        and eax, 0FFFFh
        and edx, 0FFFFh
        mov ecx, 1
        test edx, 8000h
        je L43BB65
        or edx, 0FFFF0000h
        neg ecx
        cmp eax, 1
        sbb edx, -1
L43BB65:
        add ecx, edx
        push ecx
        push edx
        sub edi, esi
        mov edx, edi
        xor eax, eax
        shrd eax, edx, 10h
        shl ebx, 10h
        sar edx, 10h
        idiv ebx
        mov dword ptr [ebp-0CCh], eax
        shld edx, eax, 10h
        and eax, 0FFFFh
        and edx, 0FFFFh
        mov ecx, dword ptr [ebp-54h]
        test edx, 8000h
        je L43BBA3
        neg ecx
        cmp eax, 1
        sbb edx, -1
L43BBA3:
        mov eax, dword ptr [ebp-54h]
        imul dx
        cwde
        pop edx
        pop ebx
        add edx, eax
        mov dword ptr [anRLESourceSteps], edx
        add edx, ecx
        mov dword ptr [anRLESourceSteps+4], edx
        add ebx, eax
        mov dword ptr [anRLESourceSteps+8], ebx
        add ebx, ecx
        mov dword ptr [anRLESourceSteps+0Ch], ebx
        mov ecx, dword ptr [ebp-0D4h]
        sub ecx, dword ptr [ebp-0C0h]
        jg L43BE33
L43BBDE:
        mov eax, dword ptr [ebp-0C4h]
        sub eax, dword ptr [ebp-0DCh]
        jg L43BE61
L43BBF0:
        mov ecx, esi
        shr esi, 10h
        mov eax, esi
        mul dword ptr [ebp-54h]
        add eax, dword ptr [ebp-58h]
        mov esi, dword ptr [ebp-0D0h]
        shr esi, 10h
        add esi, eax
        mov eax, dword ptr [ebp-0C0h]
        mov edi, dword ptr [ebp-5Ch]
        add edi, eax
        mov ebx, dword ptr [ebp-0C4h]
        sub ebx, eax
        push ebp
        mov edx, dword ptr [ebp-0D0h]
        mov eax, dword ptr [ebp-0C8h]
        or eax, eax
        jns L43BC30
        neg eax
        not edx
L43BC30:
        shl eax, 10h
        shl edx, 10h
        mov ebp, dword ptr [ebp-0CCh]
        or ebp, ebp
        jns L43BC44
        neg ebp
        not ecx
L43BC44:
        shl ebp, 10h
        shl ecx, 10h
        push ebx
        xor ebx, ebx
        cmp dword ptr [esp], 5
        jl L43BD0F
L43BC57:
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BC60
        mov byte ptr [edi], bl
L43BC60:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BC7B
        mov byte ptr [edi+1], bl
L43BC7B:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BC96
        mov byte ptr [edi+2], bl
L43BC96:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BCB1
        mov byte ptr [edi+3], bl
L43BCB1:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BCCC
        mov byte ptr [edi+4], bl
L43BCCC:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BCE7
        mov byte ptr [edi+5], bl
L43BCE7:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        add edi, 6
        sub dword ptr [esp], 6
        js L43BDA9
        cmp dword ptr [esp], 5
        jge L43BC57
L43BD0F:
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BD18
        mov byte ptr [edi], bl
L43BD18:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        dec dword ptr [esp]
        js L43BDA9
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BD38
        mov byte ptr [edi+1], bl
L43BD38:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        dec dword ptr [esp]
        js L43BDA9
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BD58
        mov byte ptr [edi+2], bl
L43BD58:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        dec dword ptr [esp]
        js L43BDA9
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BD78
        mov byte ptr [edi+3], bl
L43BD78:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
        dec dword ptr [esp]
        js L43BDA9
        mov bl, byte ptr [esi]
        cmp bl, 0FFh
        je L43BD98
        mov byte ptr [edi+4], bl
L43BD98:
        xor ebx, ebx
        add edx, eax
        adc ebx, ebx
        add ecx, ebp
        adc ebx, ebx
        add esi, dword ptr [ebx*4+anRLESourceSteps]
L43BDA9:
        add esp, 4
        pop ebp
L43BDAD:
        mov edi, dword ptr [ebp-0E8h]
        add dword ptr [ebp-5Ch], edi
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        dec dword ptr [ebp-88h]
        js L43BE04
        je L43BE0A
        dec dword ptr [ebp-80h]
        je L43BE6C
        add eax, dword ptr [ebp-0A8h]
        add ecx, dword ptr [ebp-0B0h]
        add esi, dword ptr [ebp-0B8h]
L43BDE1:
        dec dword ptr [ebp-84h]
        je L43BF1C
        add ebx, dword ptr [ebp-0ACh]
        add edx, dword ptr [ebp-0B4h]
        add edi, dword ptr [ebp-0BCh]
        jmp L43BADC
L43BE04:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43BE0A:
        add eax, dword ptr [ebp-0A8h]
        add ecx, dword ptr [ebp-0B0h]
        add esi, dword ptr [ebp-0B8h]
        add ebx, dword ptr [ebp-0ACh]
        add edx, dword ptr [ebp-0B4h]
        add edi, dword ptr [ebp-0BCh]
        jmp L43BADC
L43BE33:
        add dword ptr [ebp-0C0h], ecx
        shl ecx, 10h
        mov eax, dword ptr [ebp-0C8h]
        imul ecx
        shrd eax, edx, 10h
        add dword ptr [ebp-0D0h], eax
        mov eax, dword ptr [ebp-0CCh]
        imul ecx
        shrd eax, edx, 10h
        add esi, eax
        jmp L43BBDE
L43BE61:
        sub dword ptr [ebp-0C4h], eax
        jmp L43BBF0
L43BE6C:
        push ebx
        push edx
        mov ebx, dword ptr [ebp-78h]
        mov dword ptr [ebp-70h], ebx
        mov esi, ebx
        sub esi, 14h
        cmp esi, dword ptr [ebp-64h]
        jge L43BE84
        mov esi, dword ptr [ebp-68h]
        sub esi, 14h
L43BE84:
        mov dword ptr [ebp-78h], esi
        mov ecx, dword ptr [esi+4]
        mov edx, dword ptr [ebx+4]
        sub ecx, edx
        cmp ecx, 1
        adc ecx, 0
        mov dword ptr [ebp-80h], ecx
        mov edx, dword ptr [esi]
        sub edx, dword ptr [ebx]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0A8h], eax
        mov ecx, dword ptr [ebp-80h]
        mov edx, dword ptr [esi+0Ch]
        sub edx, dword ptr [ebx+0Ch]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0B0h], eax
        mov ecx, dword ptr [ebp-80h]
        mov edx, dword ptr [esi+10h]
        sub edx, dword ptr [ebx+10h]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0B8h], eax
        mov eax, dword ptr [ebx]
        shl eax, 10h
        add eax, 8000h
        mov ecx, dword ptr [ebx+0Ch]
        shl ecx, 10h
        add ecx, 8000h
        mov esi, dword ptr [ebx+10h]
        shl esi, 10h
        add esi, 8000h
        pop edx
        pop ebx
        jmp L43BDE1
L43BF1C:
        push eax
        push ecx
        mov ebx, dword ptr [ebp-7Ch]
        mov dword ptr [ebp-74h], ebx
        mov edi, ebx
        add edi, 14h
        cmp edi, dword ptr [ebp-68h]
        jl L43BF31
        mov edi, dword ptr [ebp-64h]
L43BF31:
        mov dword ptr [ebp-7Ch], edi
        mov ecx, dword ptr [edi+4]
        mov edx, dword ptr [ebx+4]
        sub ecx, edx
        cmp ecx, 1
        adc ecx, 0
        mov dword ptr [ebp-84h], ecx
        mov edx, dword ptr [edi]
        sub edx, dword ptr [ebx]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0ACh], eax
        mov ecx, dword ptr [ebp-84h]
        mov edx, dword ptr [edi+0Ch]
        sub edx, dword ptr [ebx+0Ch]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0B4h], eax
        mov ecx, dword ptr [ebp-84h]
        mov edx, dword ptr [edi+10h]
        sub edx, dword ptr [ebx+10h]
        shl edx, 10h
        xor eax, eax
        shrd eax, edx, 10h
        shl ecx, 10h
        sar edx, 10h
        idiv ecx
        mov dword ptr [ebp-0BCh], eax
        mov edx, dword ptr [ebx+0Ch]
        shl edx, 10h
        add edx, 8000h
        mov edi, dword ptr [ebx+10h]
        shl edi, 10h
        add edi, 8000h
        mov ebx, dword ptr [ebx]
        shl ebx, 10h
        add ebx, 8000h
        pop ecx
        pop eax
        jmp L43BADC
L43BFD3:
        test dword ptr [ebp+2Ch], 1
        je L43BFF8
        push dword ptr [ebp+18h]
        push dword ptr [ebp+14h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push dword ptr [ebp+8]
        call DrawRLEImageColor
        add esp, 14h
        jmp L43BE04
L43BFF8:
        push dword ptr [ebp+18h]
        push dword ptr [ebp+14h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push dword ptr [ebp+8]
        call DrawRLEImage
        add esp, 14h
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
RotateRLEImage endp

; Function start: 0x0043C015
; unsigned int GetRLEFrameBounds(unsigned char *shape, int frame, int x, int
;                                y, unsigned int flags, int *bounds);
; Measures the bounds of an RLE frame.
GetRLEFrameBounds proc
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
        mov dword ptr [ebp-18h], 0
        mov dword ptr [ebp-1Ch], 0
        mov dword ptr [ebp-20h], 0
        mov dword ptr [ebp-24h], 0
        mov esi, dword ptr [ebp+0Ch]
        shl esi, 3
        add esi, 8
        add esi, dword ptr [ebp+8]
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+8]
        mov dword ptr [ebp-4], esi
        mov esi, dword ptr [ebp-4]
        mov eax, dword ptr [esi+8]
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [esi+0Ch]
        mov ebx, eax
        add eax, dword ptr [ebp+14h]
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+10h]
        add eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [esi+14h]
        mov ecx, eax
        add ecx, dword ptr [ebp+14h]
        mov dword ptr [ebp-14h], ecx
        inc eax
        sub eax, ebx
        mov ebx, eax
        jle L43C13A
        add esi, 18h
        mov dword ptr [ebp-18h], 7FFFFFFFh
        mov dword ptr [ebp-1Ch], 7FFFFFFFh
        mov dword ptr [ebp-20h], 80000000h
        mov dword ptr [ebp-24h], 80000000h
        mov edx, dword ptr [ebp-0Ch]
L43C0AA:
        mov edi, dword ptr [ebp-8]
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C0CD
        jne L43C100
        jae L43C132
L43C0B8:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C0CD
        jne L43C100
        jb L43C0B8
        jae L43C132
L43C0CD:
        movzx ecx, al
        mov al, byte ptr [esi]
        inc esi
        cmp dword ptr [ebp-18h], edi
        jl L43C0DB
        mov dword ptr [ebp-18h], edi
L43C0DB:
        add edi, ecx
        cmp dword ptr [ebp-20h], edi
        jg L43C0E5
        mov dword ptr [ebp-20h], edi
L43C0E5:
        cmp dword ptr [ebp-1Ch], edx
        jl L43C0ED
        mov dword ptr [ebp-1Ch], edx
L43C0ED:
        cmp dword ptr [ebp-24h], edx
        jg L43C0F5
        mov dword ptr [ebp-24h], edx
L43C0F5:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C0CD
        jae L43C132
        je L43C0B8
L43C100:
        movzx ecx, al
        add esi, ecx
        cmp dword ptr [ebp-18h], edi
        jl L43C10D
        mov dword ptr [ebp-18h], edi
L43C10D:
        add edi, ecx
        cmp dword ptr [ebp-20h], edi
        jg L43C117
        mov dword ptr [ebp-20h], edi
L43C117:
        cmp dword ptr [ebp-1Ch], edx
        jl L43C11F
        mov dword ptr [ebp-1Ch], edx
L43C11F:
        cmp dword ptr [ebp-24h], edx
        jg L43C127
        mov dword ptr [ebp-24h], edx
L43C127:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C0CD
        jne L43C100
        jb L43C0B8
L43C132:
        inc edx
        dec ebx
        jne L43C0AA
L43C13A:
        mov eax, dword ptr [ebp+18h]
        and eax, 1
        je L43C156
        mov eax, dword ptr [ebp+10h]
        add eax, dword ptr [ebp+10h]
        mov ebx, eax
        sub eax, dword ptr [ebp-18h]
        sub ebx, dword ptr [ebp-20h]
        mov dword ptr [ebp-20h], eax
        mov dword ptr [ebp-18h], ebx
L43C156:
        mov eax, dword ptr [ebp+18h]
        and eax, 2
        je L43C172
        mov eax, dword ptr [ebp+14h]
        add eax, dword ptr [ebp+14h]
        mov ebx, eax
        sub eax, dword ptr [ebp-1Ch]
        sub ebx, dword ptr [ebp-24h]
        mov dword ptr [ebp-24h], eax
        mov dword ptr [ebp-1Ch], ebx
L43C172:
        mov edi, dword ptr [ebp+1Ch]
        mov eax, dword ptr [ebp-18h]
        stosd
        mov eax, dword ptr [ebp-1Ch]
        stosd
        mov eax, dword ptr [ebp-20h]
        stosd
        mov eax, dword ptr [ebp-24h]
        stosd
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRLEFrameBounds endp

; Function start: 0x0043C18D
; int EncodeRasterClipToRLEFrame(RasterClip *clip, unsigned char
;                                transparentColour, int originX, int originY,
;                                unsigned char *output);
; Encodes a clip into an RLE frame.
EncodeRasterClipToRLEFrame proc
        push ebp
        mov ebp, esp
        add esp, -38h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-30h], eax
        jle L43C20C
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43C20C
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-34h], eax
        cmp eax, 0
        jg L43C1C0
        mov eax, 0
L43C1C0:
        mov dword ptr [ebp-1Ch], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-38h], eax
        cmp eax, 0
        jg L43C1D3
        mov eax, 0
L43C1D3:
        mov dword ptr [ebp-20h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-30h]
        dec edx
        cmp eax, edx
        jl L43C1E3
        mov eax, edx
L43C1E3:
        mov dword ptr [ebp-24h], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43C1F2
        mov eax, edx
L43C1F2:
        mov dword ptr [ebp-28h], eax
        mov eax, dword ptr [ebp-24h]
        cmp eax, dword ptr [ebp-1Ch]
        jl L43C217
        mov eax, dword ptr [ebp-28h]
        cmp eax, dword ptr [ebp-20h]
        jl L43C217
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-2Ch], eax
        jmp L43C222
L43C20C:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43C217:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43C222:
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov edi, dword ptr [ebp+18h]
        mov dword ptr [pRLEEncodeBuffer], edi
        or edi, edi
        je L43C270
        mov eax, dword ptr [esi+0Ch]
        sub eax, dword ptr [esi+4]
        shl eax, 10h
        mov ax, word ptr [esi+10h]
        sub ax, word ptr [esi+8]
        mov dword ptr [edi], eax
        add edi, 4
        mov eax, dword ptr [ebp+10h]
        shl eax, 10h
        mov ax, word ptr [ebp+14h]
        mov dword ptr [edi], eax
        add edi, 4
        xor eax, eax
        mov dword ptr [edi], eax
        add edi, 4
        mov dword ptr [edi], eax
        add edi, 4
        dec eax
        mov dword ptr [edi], eax
        add edi, 4
        mov dword ptr [edi], eax
        add edi, 4
L43C270:
        mov eax, dword ptr [ebp-34h]
        add dword ptr [ebp+10h], eax
        mov eax, dword ptr [ebp-38h]
        add dword ptr [ebp+14h], eax
        mov eax, dword ptr [ebp-24h]
        inc eax
        sub eax, dword ptr [ebp-1Ch]
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [ebp-24h]
        inc eax
        sub eax, dword ptr [ebp-1Ch]
        mov dword ptr [ebp-0Ch], eax
        mov eax, 7FFFFFFFh
        mov dword ptr [nRLEEncodedMinimumX], eax
        mov dword ptr [nRLEEncodedMinimumY], eax
        neg eax
        mov dword ptr [nRLEEncodedMaximumX], eax
        mov dword ptr [nRLEEncodedMaximumY], eax
        mov eax, dword ptr [ebp-20h]
        imul dword ptr [ebp-30h]
        add eax, dword ptr [ebp-2Ch]
        add eax, dword ptr [ebp-1Ch]
        mov dword ptr [pRLEScanlineStart], eax
        mov esi, eax
        mov eax, dword ptr [ebp-20h]
        mov dword ptr [ebp-10h], eax
        jmp L43C360
L43C2C9:
        mov edi, dword ptr [pRLEScanlineStart]
        mov dword ptr [pRLEScanCursor], edi
        mov ecx, dword ptr [ebp-8]
        mov dword ptr [ebp-14h], ecx
        mov al, byte ptr [ebp+0Ch]
        repe scasb
        je L43C354
        mov eax, dword ptr [nRLEEncodedMinimumY]
        cmp eax, dword ptr [ebp-10h]
        jl L43C2EF
        mov eax, dword ptr [ebp-10h]
L43C2EF:
        mov dword ptr [nRLEEncodedMinimumY], eax
        mov eax, dword ptr [nRLEEncodedMaximumY]
        cmp eax, dword ptr [ebp-10h]
        jg L43C301
        mov eax, dword ptr [ebp-10h]
L43C301:
        mov dword ptr [nRLEEncodedMaximumY], eax
        mov eax, dword ptr [ebp-24h]
        sub eax, ecx
        cmp eax, dword ptr [nRLEEncodedMinimumX]
        jl L43C318
        mov eax, dword ptr [nRLEEncodedMinimumX]
L43C318:
        mov dword ptr [nRLEEncodedMinimumX], eax
        mov eax, dword ptr [pRLEScanlineStart]
        add eax, dword ptr [ebp-8]
        dec eax
        mov edi, eax
        mov dword ptr [pRLEScanCursor], edi
        mov ecx, dword ptr [ebp-8]
        mov dword ptr [ebp-14h], ecx
        mov al, byte ptr [ebp+0Ch]
        std
        repe scasb
        cld
        je L43C354
        mov eax, dword ptr [ebp-1Ch]
        add eax, ecx
        cmp eax, dword ptr [nRLEEncodedMaximumX]
        jg L43C34F
        mov eax, dword ptr [nRLEEncodedMaximumX]
L43C34F:
        mov dword ptr [nRLEEncodedMaximumX], eax
L43C354:
        mov eax, dword ptr [ebp-30h]
        add dword ptr [pRLEScanlineStart], eax
        inc dword ptr [ebp-10h]
L43C360:
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [ebp-28h]
        jle L43C2C9
        mov edi, dword ptr [ebp+18h]
        or edi, edi
        je L43C39F
        mov eax, dword ptr [nRLEEncodedMinimumX]
        sub eax, dword ptr [ebp+10h]
        mov dword ptr [edi+8], eax
        mov eax, dword ptr [nRLEEncodedMinimumY]
        sub eax, dword ptr [ebp+14h]
        mov dword ptr [edi+0Ch], eax
        mov eax, dword ptr [nRLEEncodedMaximumX]
        sub eax, dword ptr [ebp+10h]
        mov dword ptr [edi+10h], eax
        mov eax, dword ptr [nRLEEncodedMaximumY]
        sub eax, dword ptr [ebp+14h]
        mov dword ptr [edi+14h], eax
L43C39F:
        add edi, 18h
        mov dword ptr [pRLEOutputCursor], edi
        mov eax, dword ptr [nRLEEncodedMaximumX]
        inc eax
        sub eax, dword ptr [nRLEEncodedMinimumX]
        mov dword ptr [ebp-18h], eax
        mov eax, dword ptr [nRLEEncodedMinimumY]
        imul dword ptr [ebp-30h]
        add eax, dword ptr [ebp-2Ch]
        add eax, dword ptr [nRLEEncodedMinimumX]
        mov esi, eax
        mov dword ptr [pRLEScanlineStart], esi
        mov eax, dword ptr [nRLEEncodedMinimumY]
        mov dword ptr [ebp-10h], eax
        jmp L43C3F7
L43C3DA:
        push dword ptr [ebp-1Ch]
        push dword ptr [ebp+0Ch]
        push dword ptr [ebp-18h]
        call EncodeRLEScanline
        add esp, 0Ch
        mov eax, dword ptr [ebp-30h]
        add dword ptr [pRLEScanlineStart], eax
        inc dword ptr [ebp-10h]
L43C3F7:
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [nRLEEncodedMaximumY]
        jle L43C3DA
        mov eax, dword ptr [pRLEOutputCursor]
        sub eax, dword ptr [ebp+18h]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
EncodeRasterClipToRLEFrame endp

; Function start: 0x0043C410
; unsigned int TranslateRLEFramePalette(unsigned char *shape, int frame);
; Remaps the colours of an RLE frame in place.
TranslateRLEFramePalette proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+0Ch]
        shl esi, 3
        add esi, 8
        add esi, dword ptr [ebp+8]
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi+0Ch]
        mov eax, dword ptr [esi+14h]
        inc eax
        sub eax, ebx
        mov ebx, eax
        jle L43C49A
        add esi, 18h
L43C43B:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C459
        jne L43C477
        jae L43C497
L43C446:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C459
        jne L43C477
        jb L43C446
        jae L43C497
L43C459:
        movzx ecx, al
        mov eax, 0
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [esi], al
        inc esi
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C459
        jae L43C497
        je L43C446
L43C477:
        movzx ecx, al
        mov eax, 0
L43C47F:
        mov al, byte ptr [esi]
        mov al, byte ptr [eax+abRasterPaletteTranslation]
        mov byte ptr [esi], al
        inc esi
        loop L43C47F
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja L43C459
        jne L43C477
        jb L43C446
L43C497:
        dec ebx
        jne L43C43B
L43C49A:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
TranslateRLEFramePalette endp

; Function start: 0x0043C4A2
; void EncodeRLEScanline(int pixelCount, unsigned char transparentColour, int
;                        sourceX);
; Classifies one scanline into RLE runs for EncodeRasterClipToRLEFrame.
EncodeRLEScanline proc
        push ebp
        mov ebp, esp
        add esp, -4
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [pRLEScanlineStart]
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 0
        push 0
        call EmitRLEScanlineRun
        add esp, 0Ch
        mov dword ptr [ebp-4], 5
        mov ecx, dword ptr [ebp+8]
        or ecx, ecx
        je L43C600
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        mov ah, al
        cmp ah, byte ptr [ebp+0Ch]
        je L43C5D1
L43C4EB:
        mov dword ptr [ebp-4], 1
        or ecx, ecx
        je L43C600
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        xor ah, al
        or al, al
        je L43C59E
        cmp ah, byte ptr [ebp+0Ch]
        jne L43C529
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 1
        push 1
        call EmitRLEScanlineRun
        add esp, 0Ch
        jmp L43C5D1
L43C529:
        or ecx, ecx
        je L43C600
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        xor ah, al
        cmp ah, byte ptr [ebp+0Ch]
        jne L43C555
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 1
        push 1
        call EmitRLEScanlineRun
        add esp, 0Ch
        jmp L43C5D1
L43C555:
        or al, al
        jne L43C529
        or ecx, ecx
        je L43C600
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        xor ah, al
        cmp ah, byte ptr [ebp+0Ch]
        jne L43C585
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 1
        push 1
        call EmitRLEScanlineRun
        add esp, 0Ch
        jmp L43C5D1
L43C585:
        or al, al
        jne L43C529
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 3
        push 1
        call EmitRLEScanlineRun
        add esp, 0Ch
L43C59E:
        mov dword ptr [ebp-4], 2
        or ecx, ecx
        je L43C600
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        je L43C59E
        xor ah, al
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 1
        push 2
        call EmitRLEScanlineRun
        add esp, 0Ch
        cmp ah, byte ptr [ebp+0Ch]
        jne L43C4EB
L43C5D1:
        mov dword ptr [ebp-4], 3
        or ecx, ecx
        je L43C600
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        je L43C5D1
        xor ah, al
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 1
        push 3
        call EmitRLEScanlineRun
        add esp, 0Ch
        jmp L43C4EB
L43C600:
        mov dword ptr [pRLEScanCursor], esi
        push dword ptr [ebp+10h]
        push 0
        push dword ptr [ebp-4]
        call EmitRLEScanlineRun
        add esp, 0Ch
        push dword ptr [ebp+10h]
        push 0
        push 4
        call EmitRLEScanlineRun
        add esp, 0Ch
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
EncodeRLEScanline endp

; Function start: 0x0043C62B
; void EmitRLEScanlineRun(int runType, int trailingCount, int sourceX);
; Emits one RLE run for EncodeRLEScanline.
EmitRLEScanlineRun proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        push eax
        push ecx
        mov esi, dword ptr [pRLELiteralStart]
        mov edi, dword ptr [pRLEOutputCursor]
        mov eax, dword ptr [ebp+8]
        cmp eax, 2
        je L43C687
        cmp eax, 1
        je L43C72E
        cmp eax, 3
        je L43C7D3
        cmp eax, 4
        je L43C7E6
        cmp eax, 0
        jne L43C7F4
        xor eax, eax
        mov dword ptr [nRLEPendingSkip], eax
        mov esi, dword ptr [pRLEScanCursor]
        mov dword ptr [pRLELiteralStart], esi
        jmp L43C7F4
L43C687:
        mov ebx, dword ptr [nRLEPendingSkip]
        or ebx, ebx
        je L43C6C6
L43C691:
        mov ecx, ebx
        cmp ecx, 0FFh
        jl L43C6A0
        mov ecx, 0FFh
L43C6A0:
        sub ebx, ecx
        cmp dword ptr [pRLEEncodeBuffer], 0
        je L43C6B7
        mov al, 1
        mov byte ptr [edi], al
        inc edi
        mov al, cl
        mov byte ptr [edi], al
        inc edi
        jmp L43C6BA
L43C6B7:
        add edi, 2
L43C6BA:
        add esi, ecx
        or ebx, ebx
        jne L43C691
        mov dword ptr [nRLEPendingSkip], ebx
L43C6C6:
        mov ebx, dword ptr [pRLEScanCursor]
        sub ebx, esi
        sub ebx, dword ptr [ebp+0Ch]
        mov eax, dword ptr [ebp+10h]
        add eax, esi
        sub eax, dword ptr [pRLEScanlineStart]
        cmp eax, dword ptr [nRLEEncodedMinimumX]
        jge L43C6E9
        mov dword ptr [nRLEEncodedMinimumX], eax
L43C6E9:
        add eax, ebx
        dec eax
        cmp eax, dword ptr [nRLEEncodedMaximumX]
        jle L43C6FB
        mov dword ptr [nRLEEncodedMaximumX], eax
        jmp L43C725
L43C6FB:
        mov ecx, ebx
        cmp ecx, 7Fh
        jl L43C707
        mov ecx, 7Fh
L43C707:
        cmp dword ptr [pRLEEncodeBuffer], 0
        je L43C71E
        mov al, cl
        add al, al
        mov byte ptr [edi], al
        inc edi
        mov al, byte ptr [esi]
        mov byte ptr [edi], al
        inc edi
        jmp L43C721
L43C71E:
        add edi, 2
L43C721:
        add esi, ecx
        sub ebx, ecx
L43C725:
        or ebx, ebx
        jne L43C6FB
        jmp L43C7F4
L43C72E:
        mov ebx, dword ptr [nRLEPendingSkip]
        or ebx, ebx
        je L43C76D
L43C738:
        mov ecx, ebx
        cmp ecx, 0FFh
        jl L43C747
        mov ecx, 0FFh
L43C747:
        sub ebx, ecx
        cmp dword ptr [pRLEEncodeBuffer], 0
        je L43C75E
        mov al, 1
        mov byte ptr [edi], al
        inc edi
        mov al, cl
        mov byte ptr [edi], al
        inc edi
        jmp L43C761
L43C75E:
        add edi, 2
L43C761:
        add esi, ecx
        or ebx, ebx
        jne L43C738
        mov dword ptr [nRLEPendingSkip], ebx
L43C76D:
        mov ebx, dword ptr [pRLEScanCursor]
        sub ebx, esi
        sub ebx, dword ptr [ebp+0Ch]
        mov eax, dword ptr [ebp+10h]
        add eax, esi
        sub eax, dword ptr [pRLEScanlineStart]
        cmp eax, dword ptr [nRLEEncodedMinimumX]
        jge L43C790
        mov dword ptr [nRLEEncodedMinimumX], eax
L43C790:
        add eax, ebx
        dec eax
        cmp eax, dword ptr [nRLEEncodedMaximumX]
        jle L43C7A2
        mov dword ptr [nRLEEncodedMaximumX], eax
        jmp L43C7CD
L43C7A2:
        mov ecx, ebx
        cmp ecx, 7Fh
        jl L43C7AE
        mov ecx, 7Fh
L43C7AE:
        mov edx, ecx
        mov al, cl
        add al, al
        inc al
        cmp dword ptr [pRLEEncodeBuffer], 0
        je L43C7C6
        mov byte ptr [edi], al
        inc edi
        rep movsb
        jmp L43C7CB
L43C7C6:
        inc edi
        add esi, ecx
        add edi, ecx
L43C7CB:
        sub ebx, edx
L43C7CD:
        or ebx, ebx
        jne L43C7A2
        jmp L43C7F4
L43C7D3:
        mov ebx, dword ptr [pRLEScanCursor]
        sub ebx, esi
        sub ebx, dword ptr [ebp+0Ch]
        mov dword ptr [nRLEPendingSkip], ebx
        jmp L43C7F4
L43C7E6:
        xor eax, eax
        cmp dword ptr [pRLEEncodeBuffer], 0
        je L43C7F3
        mov byte ptr [edi], al
L43C7F3:
        inc edi
L43C7F4:
        mov dword ptr [pRLEOutputCursor], edi
        mov dword ptr [pRLELiteralStart], esi
        pop ecx
        pop eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
EmitRLEScanlineRun endp

; Function start: 0x0043C808
; int FillRasterClip(RasterClip *clip, int colour);
; Fills a clip with one colour.
FillRasterClip proc
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-18h], eax
        jle L43C887
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43C887
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-1Ch], eax
        cmp eax, 0
        jg L43C83B
        mov eax, 0
L43C83B:
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-20h], eax
        cmp eax, 0
        jg L43C84E
        mov eax, 0
L43C84E:
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-18h]
        dec edx
        cmp eax, edx
        jl L43C85E
        mov eax, edx
L43C85E:
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43C86D
        mov eax, edx
L43C86D:
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [ebp-0Ch]
        cmp eax, dword ptr [ebp-4]
        jl L43C892
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [ebp-8]
        jl L43C892
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-14h], eax
        jmp L43C89D
L43C887:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43C892:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43C89D:
        mov eax, dword ptr [ebp-8]
        imul dword ptr [ebp-18h]
        add eax, dword ptr [ebp-14h]
        add eax, dword ptr [ebp-4]
        mov edi, eax
        mov ebx, dword ptr [ebp-0Ch]
        inc ebx
        sub ebx, dword ptr [ebp-4]
        mov esi, dword ptr [ebp-18h]
        sub esi, ebx
        mov al, byte ptr [ebp+0Ch]
        mov ah, al
        shl eax, 10h
        mov al, byte ptr [ebp+0Ch]
        mov ah, al
        mov edx, dword ptr [ebp-8]
        jmp L43C8DA
L43C8C9:
        mov ecx, ebx
        and ecx, 3
        rep stosb
        mov ecx, ebx
        shr ecx, 2
        rep stosd
        add edi, esi
        inc edx
L43C8DA:
        cmp edx, dword ptr [ebp-10h]
        jle L43C8C9
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
FillRasterClip endp

; Function start: 0x0043C8E7
; int BlitRasterClip(RasterClip *source, int sourceX, int sourceY, RasterClip
;                    *destination, int destinationX, int destinationY,
;                    unsigned int colour);
; Copies one clip into another.
BlitRasterClip proc
        push ebp
        mov ebp, esp
        add esp, 0FFFFFF64h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-70h], eax
        jle L43C969
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43C969
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-78h], eax
        cmp eax, 0
        jg L43C91D
        mov eax, 0
L43C91D:
        mov dword ptr [ebp-60h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-7Ch], eax
        cmp eax, 0
        jg L43C930
        mov eax, 0
L43C930:
        mov dword ptr [ebp-64h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-70h]
        dec edx
        cmp eax, edx
        jl L43C940
        mov eax, edx
L43C940:
        mov dword ptr [ebp-68h], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43C94F
        mov eax, edx
L43C94F:
        mov dword ptr [ebp-6Ch], eax
        mov eax, dword ptr [ebp-68h]
        cmp eax, dword ptr [ebp-60h]
        jl L43C974
        mov eax, dword ptr [ebp-6Ch]
        cmp eax, dword ptr [ebp-64h]
        jl L43C974
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-74h], eax
        jmp L43C97F
L43C969:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43C974:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43C97F:
        mov eax, dword ptr [ebp-60h]
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [ebp-64h]
        mov dword ptr [ebp-30h], eax
        mov eax, dword ptr [ebp-68h]
        mov dword ptr [ebp-34h], eax
        mov eax, dword ptr [ebp-6Ch]
        mov dword ptr [ebp-38h], eax
        mov eax, dword ptr [ebp-78h]
        sub dword ptr [ebp-2Ch], eax
        sub dword ptr [ebp-34h], eax
        mov eax, dword ptr [ebp-7Ch]
        sub dword ptr [ebp-30h], eax
        sub dword ptr [ebp-38h], eax
        mov esi, dword ptr [ebp+14h]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-94h], eax
        jle L43CA40
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43CA40
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-98h], eax
        cmp eax, 0
        jg L43C9D9
        mov eax, 0
L43C9D9:
        mov dword ptr [ebp-80h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-9Ch], eax
        cmp eax, 0
        jg L43C9EF
        mov eax, 0
L43C9EF:
        mov dword ptr [ebp-84h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-94h]
        dec edx
        cmp eax, edx
        jl L43CA05
        mov eax, edx
L43CA05:
        mov dword ptr [ebp-88h], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43CA17
        mov eax, edx
L43CA17:
        mov dword ptr [ebp-8Ch], eax
        mov eax, dword ptr [ebp-88h]
        cmp eax, dword ptr [ebp-80h]
        jl L43CA4B
        mov eax, dword ptr [ebp-8Ch]
        cmp eax, dword ptr [ebp-84h]
        jl L43CA4B
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-90h], eax
        jmp L43CA56
L43CA40:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CA4B:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CA56:
        mov eax, dword ptr [ebp-80h]
        mov dword ptr [ebp-40h], eax
        mov eax, dword ptr [ebp-84h]
        mov dword ptr [ebp-44h], eax
        mov eax, dword ptr [ebp-88h]
        mov dword ptr [ebp-48h], eax
        mov eax, dword ptr [ebp-8Ch]
        mov dword ptr [ebp-4Ch], eax
        mov eax, dword ptr [ebp-98h]
        sub dword ptr [ebp-40h], eax
        sub dword ptr [ebp-48h], eax
        mov eax, dword ptr [ebp-9Ch]
        sub dword ptr [ebp-44h], eax
        sub dword ptr [ebp-4Ch], eax
        mov eax, dword ptr [ebp+0Ch]
        sub eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-24h], eax
        mov eax, dword ptr [ebp+10h]
        sub eax, dword ptr [ebp+1Ch]
        mov dword ptr [ebp-28h], eax
        mov eax, dword ptr [ebp-2Ch]
        mov edx, dword ptr [ebp-40h]
        add edx, dword ptr [ebp-24h]
        cmp eax, edx
        jg L43CAB0
        mov eax, edx
L43CAB0:
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [ebp-30h]
        mov edx, dword ptr [ebp-44h]
        add edx, dword ptr [ebp-28h]
        cmp eax, edx
        jg L43CAC2
        mov eax, edx
L43CAC2:
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [ebp-34h]
        mov edx, dword ptr [ebp-48h]
        add edx, dword ptr [ebp-24h]
        cmp eax, edx
        jl L43CAD4
        mov eax, edx
L43CAD4:
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [ebp-38h]
        mov edx, dword ptr [ebp-4Ch]
        add edx, dword ptr [ebp-28h]
        cmp eax, edx
        jl L43CAE6
        mov eax, edx
L43CAE6:
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [ebp-0Ch]
        cmp eax, dword ptr [ebp-4]
        jl L43CC78
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [ebp-8]
        jl L43CC78
        mov eax, dword ptr [ebp-40h]
        mov edx, dword ptr [ebp-2Ch]
        sub edx, dword ptr [ebp-24h]
        cmp eax, edx
        jg L43CB10
        mov eax, edx
L43CB10:
        mov dword ptr [ebp-14h], eax
        mov eax, dword ptr [ebp-44h]
        mov edx, dword ptr [ebp-30h]
        sub edx, dword ptr [ebp-28h]
        cmp eax, edx
        jg L43CB22
        mov eax, edx
L43CB22:
        mov dword ptr [ebp-18h], eax
        mov eax, dword ptr [ebp-48h]
        mov edx, dword ptr [ebp-34h]
        sub edx, dword ptr [ebp-24h]
        cmp eax, edx
        jl L43CB34
        mov eax, edx
L43CB34:
        mov dword ptr [ebp-1Ch], eax
        mov eax, dword ptr [ebp-4Ch]
        mov edx, dword ptr [ebp-38h]
        sub edx, dword ptr [ebp-28h]
        cmp eax, edx
        jl L43CB46
        mov eax, edx
L43CB46:
        mov dword ptr [ebp-20h], eax
        mov eax, dword ptr [ebp-0Ch]
        inc eax
        sub eax, dword ptr [ebp-4]
        mov dword ptr [ebp-58h], eax
        mov eax, dword ptr [ebp-10h]
        inc eax
        sub eax, dword ptr [ebp-8]
        mov dword ptr [ebp-54h], eax
        mov eax, dword ptr [ebp-7Ch]
        imul dword ptr [ebp-70h]
        add eax, dword ptr [ebp-74h]
        add eax, dword ptr [ebp-78h]
        mov esi, eax
        mov eax, dword ptr [ebp-9Ch]
        imul dword ptr [ebp-94h]
        add eax, dword ptr [ebp-90h]
        add eax, dword ptr [ebp-98h]
        mov edi, eax
        mov eax, dword ptr [ebp-8]
        mov ebx, dword ptr [ebp-18h]
        cmp eax, ebx
        jle L43CBAF
        mul dword ptr [ebp-70h]
        add esi, eax
        mov eax, ebx
        mul dword ptr [ebp-94h]
        add edi, eax
        mov eax, dword ptr [ebp-70h]
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [ebp-94h]
        mov dword ptr [ebp-50h], eax
        jmp L43CBD5
L43CBAF:
        mov eax, dword ptr [ebp-10h]
        mul dword ptr [ebp-70h]
        add esi, eax
        mov eax, dword ptr [ebp-20h]
        mul dword ptr [ebp-94h]
        add edi, eax
        mov eax, dword ptr [ebp-70h]
        neg eax
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [ebp-94h]
        neg eax
        mov dword ptr [ebp-50h], eax
L43CBD5:
        mov ecx, dword ptr [ebp-58h]
        mov eax, dword ptr [ebp-4]
        mov ebx, dword ptr [ebp-14h]
        cmp eax, ebx
        jle L43CBF6
        add esi, eax
        add edi, ebx
        sub dword ptr [ebp-3Ch], ecx
        sub dword ptr [ebp-50h], ecx
        cld
        mov dword ptr [ebp-5Ch], 0
        jmp L43CC0A
L43CBF6:
        add esi, dword ptr [ebp-0Ch]
        add edi, dword ptr [ebp-1Ch]
        add dword ptr [ebp-3Ch], ecx
        add dword ptr [ebp-50h], ecx
        std
        mov dword ptr [ebp-5Ch], 3
L43CC0A:
        mov eax, dword ptr [ebp+20h]
        test eax, 0FFFFFF00h
        je L43CC43
        mov edx, dword ptr [ebp-54h]
        mov eax, dword ptr [ebp-3Ch]
        mov ebx, dword ptr [ebp-50h]
L43CC1D:
        mov ecx, dword ptr [ebp-58h]
        and ecx, 3
        rep movsb
        mov ecx, dword ptr [ebp-58h]
        shr ecx, 2
        sub esi, dword ptr [ebp-5Ch]
        sub edi, dword ptr [ebp-5Ch]
        rep movsd
        add esi, dword ptr [ebp-5Ch]
        add edi, dword ptr [ebp-5Ch]
        add esi, eax
        add edi, ebx
        dec edx
        jne L43CC1D
        cld
        jmp L43CC70
L43CC43:
        mov dl, al
        mov ah, al
        shl eax, 10h
        mov al, dl
        mov ah, al
        mov edx, dword ptr [ebp-54h]
        mov ebx, dword ptr [ebp-50h]
L43CC54:
        mov ecx, dword ptr [ebp-58h]
        and ecx, 3
        rep stosb
        mov ecx, dword ptr [ebp-58h]
        shr ecx, 2
        sub edi, dword ptr [ebp-5Ch]
        rep stosd
        add edi, dword ptr [ebp-5Ch]
        add edi, ebx
        dec edx
        jne L43CC54
        cld
L43CC70:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CC78:
        mov eax, 0FFFFFFFDh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
BlitRasterClip endp

; Function start: 0x0043CC83
; int ScrollRasterClipWrapped(RasterClip *clip, int deltaX, int deltaY, int
;                             mode, unsigned int fillOrScratch);
; Scrolls a clip, wrapping the rows that leave it.
ScrollRasterClipWrapped proc
        push ebp
        mov ebp, esp
        add esp, -44h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+0Ch]
        inc eax
        sub eax, dword ptr [esi+4]
        mov dword ptr [ebp-30h], eax
        jle L43CE75
        mov edx, dword ptr [esi+10h]
        inc edx
        sub edx, dword ptr [esi+8]
        mov dword ptr [ebp-34h], edx
        jle L43CE75
        neg eax
        mov dword ptr [ebp-38h], eax
        neg edx
        mov dword ptr [ebp-3Ch], edx
        cmp dword ptr [ebp+14h], 1
        je L43CD0A
        mov eax, dword ptr [ebp+0Ch]
        cdq
        xor eax, edx
        sub eax, edx
        cmp eax, dword ptr [ebp-30h]
        jge L43CCEE
        mov eax, dword ptr [ebp+10h]
        cdq
        xor eax, edx
        sub eax, edx
        cmp eax, dword ptr [ebp-34h]
        jge L43CCEE
        mov eax, dword ptr [ebp+8]
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-44h], eax
        jmp L43CD75
L43CCEE:
        mov eax, dword ptr [ebp+18h]
        push 0
        movzx ax, al
        push ax
        push dword ptr [ebp+8]
        call FillRasterClip
        add esp, 8
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CD0A:
        cmp dword ptr [ebp+18h], 0
        je L43CE69
        lea eax, dword ptr [ebp-28h]
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-14h], eax
        lea eax, dword ptr [ebp-14h]
        mov dword ptr [ebp-28h], eax
        xor eax, eax
        mov dword ptr [ebp-24h], eax
        mov dword ptr [ebp-20h], eax
        mov eax, dword ptr [ebp-30h]
        dec eax
        mov dword ptr [ebp-10h], eax
        mov dword ptr [ebp-1Ch], eax
        mov eax, dword ptr [ebp-34h]
        dec eax
        mov dword ptr [ebp-0Ch], eax
        mov dword ptr [ebp-18h], eax
        push -1
        push 0
        push 0
        push dword ptr [ebp-2Ch]
        push 0
        push 0
        push dword ptr [ebp+8]
        call BlitRasterClip
        add esp, 1Ch
        mov eax, dword ptr [ebp+0Ch]
        cdq
        idiv dword ptr [ebp-30h]
        mov dword ptr [ebp+0Ch], edx
        mov eax, dword ptr [ebp+10h]
        cdq
        idiv dword ptr [ebp-34h]
        mov dword ptr [ebp+10h], edx
        mov dword ptr [ebp-44h], 0FFFFFFFFh
L43CD75:
        mov eax, dword ptr [ebp+0Ch]
        or eax, dword ptr [ebp+10h]
        je L43CE61
        mov esi, dword ptr [ebp-2Ch]
        mov edi, dword ptr [ebp+8]
        push -1
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push 0
        push 0
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push dword ptr [ebp-34h]
        push dword ptr [ebp-30h]
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push 0
        push dword ptr [ebp-30h]
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push dword ptr [ebp-3Ch]
        push dword ptr [ebp-30h]
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push dword ptr [ebp-34h]
        push 0
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push dword ptr [ebp-3Ch]
        push 0
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push dword ptr [ebp-34h]
        push dword ptr [ebp-38h]
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push 0
        push dword ptr [ebp-38h]
        push esi
        call BlitRasterClip
        add esp, 1Ch
        push dword ptr [ebp-44h]
        push dword ptr [ebp+10h]
        push dword ptr [ebp+0Ch]
        push edi
        push dword ptr [ebp-3Ch]
        push dword ptr [ebp-38h]
        push esi
        call BlitRasterClip
        add esp, 1Ch
L43CE61:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CE69:
        mov eax, dword ptr [ebp-30h]
        mul dword ptr [ebp-34h]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CE75:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
ScrollRasterClipWrapped endp

; Function start: 0x0043CE80
; unsigned int DrawRasterEllipse(RasterClip *clip, int x, int y, int
;                                horizontalRadius, int verticalRadius, int
;                                colour);
; Draws an ellipse outline.
DrawRasterEllipse proc
        push ebp
        mov ebp, esp
        add esp, -54h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        cmp dword ptr [ebp+14h], 0
        je L43CE99
        cmp dword ptr [ebp+18h], 0
        jne L43CECA
L43CE99:
        mov eax, dword ptr [ebp+10h]
        add eax, dword ptr [ebp+18h]
        mov ebx, dword ptr [ebp+0Ch]
        add ebx, dword ptr [ebp+14h]
        mov ecx, dword ptr [ebp+10h]
        sub ecx, dword ptr [ebp+18h]
        mov edx, dword ptr [ebp+0Ch]
        sub edx, dword ptr [ebp+14h]
        push dword ptr [ebp+1Ch]
        push 0
        push eax
        push ebx
        push ecx
        push edx
        push dword ptr [ebp+8]
        call DrawClippedLine
        add esp, 1Ch
        jmp L43D1BB
L43CECA:
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-4Ch], eax
        jle L43CF3C
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43CF3C
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-50h], eax
        cmp eax, 0
        jg L43CEF0
        mov eax, 0
L43CEF0:
        mov dword ptr [ebp-38h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-54h], eax
        cmp eax, 0
        jg L43CF03
        mov eax, 0
L43CF03:
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-4Ch]
        dec edx
        cmp eax, edx
        jl L43CF13
        mov eax, edx
L43CF13:
        mov dword ptr [ebp-40h], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43CF22
        mov eax, edx
L43CF22:
        mov dword ptr [ebp-44h], eax
        mov eax, dword ptr [ebp-40h]
        cmp eax, dword ptr [ebp-38h]
        jl L43CF47
        mov eax, dword ptr [ebp-44h]
        cmp eax, dword ptr [ebp-3Ch]
        jl L43CF47
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-48h], eax
        jmp L43CF52
L43CF3C:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CF47:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43CF52:
        mov eax, dword ptr [ebp+1Ch]
        mov ah, al
        mov dword ptr [ebp+1Ch], eax
        mov word ptr [ebp+1Eh], ax
        mov eax, dword ptr [ebp-50h]
        add dword ptr [ebp+0Ch], eax
        mov eax, dword ptr [ebp-54h]
        add dword ptr [ebp+10h], eax
        mov eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-8], eax
        mov dword ptr [ebp-0Ch], 0
        mov eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-10h], eax
        mul eax
        mov dword ptr [ebp-1Ch], eax
        shl eax, 1
        mov dword ptr [ebp-20h], eax
        mov eax, dword ptr [ebp+14h]
        mul eax
        mov dword ptr [ebp-14h], eax
        shl eax, 1
        mov dword ptr [ebp-18h], eax
        mov dword ptr [ebp-24h], 0
        mov eax, dword ptr [ebp-18h]
        mul dword ptr [ebp+18h]
        mov dword ptr [ebp-28h], eax
        mov eax, dword ptr [ebp-14h]
        shr eax, 2
        add eax, dword ptr [ebp-1Ch]
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [ebp-14h]
        mul dword ptr [ebp+18h]
        sub dword ptr [ebp-2Ch], eax
        mov ebx, dword ptr [ebp+18h]
L43CFC2:
        mov eax, dword ptr [ebp-24h]
        sub eax, dword ptr [ebp-28h]
        jns L43D0B8
        push ebx
        mov ecx, dword ptr [ebp+1Ch]
        mov edi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        add edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D000
        cmp edi, dword ptr [ebp-40h]
        jg L43D000
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D000
        cmp edx, dword ptr [ebp-44h]
        jg L43D000
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D000:
        mov edi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        sub edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D02E
        cmp edi, dword ptr [ebp-40h]
        jg L43D02E
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D02E
        cmp edx, dword ptr [ebp-44h]
        jg L43D02E
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D02E:
        mov edi, dword ptr [ebp-4]
        sub edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        add edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D05C
        cmp edi, dword ptr [ebp-40h]
        jg L43D05C
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D05C
        cmp edx, dword ptr [ebp-44h]
        jg L43D05C
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D05C:
        mov edi, dword ptr [ebp-4]
        sub edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        sub edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D08A
        cmp edi, dword ptr [ebp-40h]
        jg L43D08A
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D08A
        cmp edx, dword ptr [ebp-44h]
        jg L43D08A
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D08A:
        pop ebx
        cmp dword ptr [ebp-2Ch], 0
        js L43D0A1
        dec dword ptr [ebp-10h]
        dec ebx
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [ebp-18h]
        mov dword ptr [ebp-28h], eax
        sub dword ptr [ebp-2Ch], eax
L43D0A1:
        inc dword ptr [ebp-0Ch]
        mov eax, dword ptr [ebp-24h]
        add eax, dword ptr [ebp-20h]
        mov dword ptr [ebp-24h], eax
        add eax, dword ptr [ebp-1Ch]
        add dword ptr [ebp-2Ch], eax
        jmp L43CFC2
L43D0B8:
        mov eax, dword ptr [ebp-14h]
        sub eax, dword ptr [ebp-1Ch]
        mov edx, eax
        sar eax, 1
        add eax, edx
        sub eax, dword ptr [ebp-24h]
        sub eax, dword ptr [ebp-28h]
        sar eax, 1
        add dword ptr [ebp-2Ch], eax
L43D0CF:
        push ebx
        mov ecx, dword ptr [ebp+1Ch]
        mov edi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        add edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D101
        cmp edi, dword ptr [ebp-40h]
        jg L43D101
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D101
        cmp edx, dword ptr [ebp-44h]
        jg L43D101
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D101:
        mov edi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        sub edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D12F
        cmp edi, dword ptr [ebp-40h]
        jg L43D12F
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D12F
        cmp edx, dword ptr [ebp-44h]
        jg L43D12F
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D12F:
        mov edi, dword ptr [ebp-4]
        sub edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        add edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D15D
        cmp edi, dword ptr [ebp-40h]
        jg L43D15D
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D15D
        cmp edx, dword ptr [ebp-44h]
        jg L43D15D
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D15D:
        mov edi, dword ptr [ebp-4]
        sub edi, dword ptr [ebp-0Ch]
        mov edx, dword ptr [ebp-8]
        sub edx, dword ptr [ebp-10h]
        cmp edi, dword ptr [ebp-38h]
        jl L43D18B
        cmp edi, dword ptr [ebp-40h]
        jg L43D18B
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D18B
        cmp edx, dword ptr [ebp-44h]
        jg L43D18B
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov byte ptr [edi], cl
L43D18B:
        pop ebx
        cmp dword ptr [ebp-2Ch], 0
        jns L43D1A1
        inc dword ptr [ebp-0Ch]
        mov eax, dword ptr [ebp-24h]
        add eax, dword ptr [ebp-20h]
        mov dword ptr [ebp-24h], eax
        add dword ptr [ebp-2Ch], eax
L43D1A1:
        dec dword ptr [ebp-10h]
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [ebp-18h]
        mov dword ptr [ebp-28h], eax
        sub eax, dword ptr [ebp-14h]
        sub dword ptr [ebp-2Ch], eax
        dec ebx
        js L43D1BB
        jmp L43D0CF
L43D1BB:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DrawRasterEllipse endp

; Function start: 0x0043D1C1
; unsigned int FillRasterEllipse(RasterClip *clip, int x, int y, int
;                                horizontalRadius, int verticalRadius, int
;                                colour);
; Fills an ellipse.
FillRasterEllipse proc
        push ebp
        mov ebp, esp
        add esp, -54h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        cmp dword ptr [ebp+14h], 0
        je L43D1DA
        cmp dword ptr [ebp+18h], 0
        jne L43D20B
L43D1DA:
        mov eax, dword ptr [ebp+10h]
        add eax, dword ptr [ebp+18h]
        mov ebx, dword ptr [ebp+0Ch]
        add ebx, dword ptr [ebp+14h]
        mov ecx, dword ptr [ebp+10h]
        sub ecx, dword ptr [ebp+18h]
        mov edx, dword ptr [ebp+0Ch]
        sub edx, dword ptr [ebp+14h]
        push dword ptr [ebp+1Ch]
        push 0
        push eax
        push ebx
        push ecx
        push edx
        push dword ptr [ebp+8]
        call DrawClippedLine
        add esp, 1Ch
        jmp L43D4B9
L43D20B:
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-4Ch], eax
        jle L43D27D
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43D27D
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-50h], eax
        cmp eax, 0
        jg L43D231
        mov eax, 0
L43D231:
        mov dword ptr [ebp-38h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-54h], eax
        cmp eax, 0
        jg L43D244
        mov eax, 0
L43D244:
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-4Ch]
        dec edx
        cmp eax, edx
        jl L43D254
        mov eax, edx
L43D254:
        mov dword ptr [ebp-40h], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43D263
        mov eax, edx
L43D263:
        mov dword ptr [ebp-44h], eax
        mov eax, dword ptr [ebp-40h]
        cmp eax, dword ptr [ebp-38h]
        jl L43D288
        mov eax, dword ptr [ebp-44h]
        cmp eax, dword ptr [ebp-3Ch]
        jl L43D288
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-48h], eax
        jmp L43D293
L43D27D:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43D288:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43D293:
        mov eax, dword ptr [ebp+1Ch]
        mov ah, al
        mov dword ptr [ebp+1Ch], eax
        mov word ptr [ebp+1Eh], ax
        mov eax, dword ptr [ebp-50h]
        add dword ptr [ebp+0Ch], eax
        mov eax, dword ptr [ebp-54h]
        add dword ptr [ebp+10h], eax
        mov eax, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [ebp+10h]
        mov dword ptr [ebp-8], eax
        mov dword ptr [ebp-0Ch], 0
        mov eax, dword ptr [ebp+18h]
        mov dword ptr [ebp-10h], eax
        mul eax
        mov dword ptr [ebp-1Ch], eax
        shl eax, 1
        mov dword ptr [ebp-20h], eax
        mov eax, dword ptr [ebp+14h]
        mul eax
        mov dword ptr [ebp-14h], eax
        shl eax, 1
        mov dword ptr [ebp-18h], eax
        mov dword ptr [ebp-24h], 0
        mov eax, dword ptr [ebp-18h]
        mul dword ptr [ebp+18h]
        mov dword ptr [ebp-28h], eax
        mov eax, dword ptr [ebp-14h]
        shr eax, 2
        add eax, dword ptr [ebp-1Ch]
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [ebp-14h]
        mul dword ptr [ebp+18h]
        sub dword ptr [ebp-2Ch], eax
        mov ebx, dword ptr [ebp+18h]
L43D303:
        mov eax, dword ptr [ebp-24h]
        sub eax, dword ptr [ebp-28h]
        js L43D310
        jmp L43D3D8
L43D310:
        mov edi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        cmp edi, dword ptr [ebp-38h]
        jl L43D3AB
        cmp edi, dword ptr [ebp-40h]
        jl L43D327
        mov edi, dword ptr [ebp-40h]
L43D327:
        mov dword ptr [ebp-34h], edi
        mov edi, dword ptr [ebp-4]
        sub edi, dword ptr [ebp-0Ch]
        cmp edi, dword ptr [ebp-40h]
        jg L43D3AB
        cmp edi, dword ptr [ebp-38h]
        jg L43D33D
        mov edi, dword ptr [ebp-38h]
L43D33D:
        mov dword ptr [ebp-30h], edi
        mov edx, dword ptr [ebp-8]
        add edx, dword ptr [ebp-10h]
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D3AB
        cmp edx, dword ptr [ebp-44h]
        jg L43D374
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp-34h]
        sub ecx, dword ptr [ebp-30h]
        inc ecx
        mov eax, dword ptr [ebp+1Ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
L43D374:
        mov edi, dword ptr [ebp-30h]
        mov edx, dword ptr [ebp-8]
        sub edx, dword ptr [ebp-10h]
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D3AB
        cmp edx, dword ptr [ebp-44h]
        jg L43D3AB
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp-34h]
        sub ecx, dword ptr [ebp-30h]
        inc ecx
        mov eax, dword ptr [ebp+1Ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
L43D3AB:
        cmp dword ptr [ebp-2Ch], 0
        js L43D3C1
        dec dword ptr [ebp-10h]
        dec ebx
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [ebp-18h]
        mov dword ptr [ebp-28h], eax
        sub dword ptr [ebp-2Ch], eax
L43D3C1:
        inc dword ptr [ebp-0Ch]
        mov eax, dword ptr [ebp-24h]
        add eax, dword ptr [ebp-20h]
        mov dword ptr [ebp-24h], eax
        add eax, dword ptr [ebp-1Ch]
        add dword ptr [ebp-2Ch], eax
        jmp L43D303
L43D3D8:
        mov eax, dword ptr [ebp-14h]
        sub eax, dword ptr [ebp-1Ch]
        mov edx, eax
        sar eax, 1
        add eax, edx
        sub eax, dword ptr [ebp-24h]
        sub eax, dword ptr [ebp-28h]
        sar eax, 1
        add dword ptr [ebp-2Ch], eax
L43D3EF:
        mov edi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        cmp edi, dword ptr [ebp-38h]
        jl L43D48A
        cmp edi, dword ptr [ebp-40h]
        jl L43D406
        mov edi, dword ptr [ebp-40h]
L43D406:
        mov dword ptr [ebp-34h], edi
        mov edi, dword ptr [ebp-4]
        sub edi, dword ptr [ebp-0Ch]
        cmp edi, dword ptr [ebp-40h]
        jg L43D48A
        cmp edi, dword ptr [ebp-38h]
        jg L43D41C
        mov edi, dword ptr [ebp-38h]
L43D41C:
        mov dword ptr [ebp-30h], edi
        mov edx, dword ptr [ebp-8]
        add edx, dword ptr [ebp-10h]
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D48A
        cmp edx, dword ptr [ebp-44h]
        jg L43D453
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp-34h]
        sub ecx, dword ptr [ebp-30h]
        inc ecx
        mov eax, dword ptr [ebp+1Ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
L43D453:
        mov edi, dword ptr [ebp-30h]
        mov edx, dword ptr [ebp-8]
        sub edx, dword ptr [ebp-10h]
        cmp edx, dword ptr [ebp-3Ch]
        jl L43D48A
        cmp edx, dword ptr [ebp-44h]
        jg L43D48A
        mov eax, edx
        imul dword ptr [ebp-4Ch]
        add eax, dword ptr [ebp-48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp-34h]
        sub ecx, dword ptr [ebp-30h]
        inc ecx
        mov eax, dword ptr [ebp+1Ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
L43D48A:
        cmp dword ptr [ebp-2Ch], 0
        jns L43D49F
        inc dword ptr [ebp-0Ch]
        mov eax, dword ptr [ebp-24h]
        add eax, dword ptr [ebp-20h]
        mov dword ptr [ebp-24h], eax
        add dword ptr [ebp-2Ch], eax
L43D49F:
        dec dword ptr [ebp-10h]
        mov eax, dword ptr [ebp-28h]
        sub eax, dword ptr [ebp-18h]
        mov dword ptr [ebp-28h], eax
        sub eax, dword ptr [ebp-14h]
        sub dword ptr [ebp-2Ch], eax
        dec ebx
        js L43D4B9
        jmp L43D3EF
L43D4B9:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
FillRasterEllipse endp

; Quarter-wave cosine table, 901 entries, 16.16 fixed point.
anRLEQuarterCosine dd 10000h, 10000h, 10000h, 0FFFFh, 0FFFEh, 0FFFEh, 0FFFCh, 0FFFBh
                   dd 0FFFAh, 0FFF8h, 0FFF6h, 0FFF4h, 0FFF2h, 0FFEFh, 0FFECh, 0FFEAh
                   dd 0FFE6h, 0FFE3h, 0FFE0h, 0FFDCh, 0FFD8h, 0FFD4h, 0FFD0h, 0FFCBh
                   dd 0FFC7h, 0FFC2h, 0FFBDh, 0FFB7h, 0FFB2h, 0FFACh, 0FFA6h, 0FFA0h
                   dd 0FF9Ah, 0FF93h, 0FF8Dh, 0FF86h, 0FF7Fh, 0FF77h, 0FF70h, 0FF68h
                   dd 0FF60h, 0FF58h, 0FF50h, 0FF48h, 0FF3Fh, 0FF36h, 0FF2Dh, 0FF24h
                   dd 0FF1Ah, 0FF10h, 0FF07h, 0FEFDh, 0FEF2h, 0FEE8h, 0FEDDh, 0FED2h
                   dd 0FEC7h, 0FEBCh, 0FEB1h, 0FEA5h, 0FE99h, 0FE8Dh, 0FE81h, 0FE74h
                   dd 0FE68h, 0FE5Bh, 0FE4Eh, 0FE40h, 0FE33h, 0FE25h, 0FE18h, 0FE09h
                   dd 0FDFBh, 0FDEDh, 0FDDEh, 0FDCFh, 0FDC0h, 0FDB1h, 0FDA2h, 0FD92h
                   dd 0FD82h, 0FD72h, 0FD62h, 0FD52h, 0FD41h, 0FD30h, 0FD1Fh, 0FD0Eh
                   dd 0FCFDh, 0FCEBh, 0FCD9h, 0FCC7h, 0FCB5h, 0FCA3h, 0FC90h, 0FC7Dh
                   dd 0FC6Ah, 0FC57h, 0FC44h, 0FC30h, 0FC1Ch, 0FC08h, 0FBF4h, 0FBE0h
                   dd 0FBCBh, 0FBB7h, 0FBA2h, 0FB8Dh, 0FB77h, 0FB62h, 0FB4Ch, 0FB36h
                   dd 0FB20h, 0FB0Ah, 0FAF3h, 0FADCh, 0FAC5h, 0FAAEh, 0FA97h, 0FA80h
                   dd 0FA68h, 0FA50h, 0FA38h, 0FA20h, 0FA07h, 0F9EFh, 0F9D6h, 0F9BDh
                   dd 0F9A3h, 0F98Ah, 0F970h, 0F956h, 0F93Ch, 0F922h, 0F908h, 0F8EDh
                   dd 0F8D2h, 0F8B7h, 0F89Ch, 0F881h, 0F865h, 0F84Ah, 0F82Eh, 0F811h
                   dd 0F7F5h, 0F7D9h, 0F7BCh, 0F79Fh, 0F782h, 0F764h, 0F747h, 0F729h
                   dd 0F70Bh, 0F6EDh, 0F6CFh, 0F6B0h, 0F692h, 0F673h, 0F654h, 0F635h
                   dd 0F615h, 0F5F6h, 0F5D6h, 0F5B6h, 0F596h, 0F575h, 0F555h, 0F534h
                   dd 0F513h, 0F4F2h, 0F4D0h, 0F4AFh, 0F48Dh, 0F46Bh, 0F449h, 0F427h
                   dd 0F404h, 0F3E2h, 0F3BFh, 0F39Ch, 0F378h, 0F355h, 0F331h, 0F30Eh
                   dd 0F2EAh, 0F2C5h, 0F2A1h, 0F27Ch, 0F258h, 0F233h, 0F20Eh, 0F1E8h
                   dd 0F1C3h, 0F19Dh, 0F177h, 0F151h, 0F12Bh, 0F104h, 0F0DEh, 0F0B7h
                   dd 0F090h, 0F068h, 0F041h, 0F019h, 0EFF2h, 0EFCAh, 0EFA2h, 0EF79h
                   dd 0EF51h, 0EF28h, 0EEFFh, 0EED6h, 0EEADh, 0EE83h, 0EE5Ah, 0EE30h
                   dd 0EE06h, 0EDDCh, 0EDB1h, 0ED87h, 0ED5Ch, 0ED31h, 0ED06h, 0ECDBh
                   dd 0ECAFh, 0EC83h, 0EC58h, 0EC2Bh, 0EBFFh, 0EBD3h, 0EBA6h, 0EB79h
                   dd 0EB4Ch, 0EB1Fh, 0EAF2h, 0EAC4h, 0EA97h, 0EA69h, 0EA3Bh, 0EA0Dh
                   dd 0E9DEh, 0E9B0h, 0E981h, 0E952h, 0E923h, 0E8F3h, 0E8C4h, 0E894h
                   dd 0E864h, 0E834h, 0E804h, 0E7D3h, 0E7A3h, 0E772h, 0E741h, 0E710h
                   dd 0E6DEh, 0E6ADh, 0E67Bh, 0E649h, 0E617h, 0E5E5h, 0E5B3h, 0E580h
                   dd 0E54Dh, 0E51Ah, 0E4E7h, 0E4B4h, 0E481h, 0E44Dh, 0E419h, 0E3E5h
                   dd 0E3B1h, 0E37Ch, 0E348h, 0E313h, 0E2DEh, 0E2A9h, 0E274h, 0E23Eh
                   dd 0E209h, 0E1D3h, 0E19Dh, 0E167h, 0E131h, 0E0FAh, 0E0C3h, 0E08Dh
                   dd 0E056h, 0E01Eh, 0DFE7h, 0DFB0h, 0DF78h, 0DF40h, 0DF08h, 0DED0h
                   dd 0DE97h, 0DE5Fh, 0DE26h, 0DDEDh, 0DDB4h, 0DD7Bh, 0DD41h, 0DD07h
                   dd 0DCCEh, 0DC94h, 0DC5Ah, 0DC1Fh, 0DBE5h, 0DBAAh, 0DB6Fh, 0DB34h
                   dd 0DAF9h, 0DABEh, 0DA82h, 0DA47h, 0DA0Bh, 0D9CFh, 0D993h, 0D956h
                   dd 0D91Ah, 0D8DDh, 0D8A0h, 0D863h, 0D826h, 0D7E9h, 0D7ABh, 0D76Dh
                   dd 0D72Fh, 0D6F1h, 0D6B3h, 0D675h, 0D636h, 0D5F7h, 0D5B9h, 0D57Ah
                   dd 0D53Ah, 0D4FBh, 0D4BBh, 0D47Ch, 0D43Ch, 0D3FCh, 0D3BCh, 0D37Bh
                   dd 0D33Bh, 0D2FAh, 0D2B9h, 0D278h, 0D237h, 0D1F5h, 0D1B4h, 0D172h
                   dd 0D130h, 0D0EEh, 0D0ACh, 0D06Ah, 0D027h, 0CFE5h, 0CFA2h, 0CF5Fh
                   dd 0CF1Ch, 0CED8h, 0CE95h, 0CE51h, 0CE0Eh, 0CDCAh, 0CD85h, 0CD41h
                   dd 0CCFDh, 0CCB8h, 0CC73h, 0CC2Eh, 0CBE9h, 0CBA4h, 0CB5Fh, 0CB19h
                   dd 0CAD3h, 0CA8Eh, 0CA48h, 0CA01h, 0C9BBh, 0C975h, 0C92Eh, 0C8E7h
                   dd 0C8A0h, 0C859h, 0C812h, 0C7CAh, 0C783h, 0C73Bh, 0C6F3h, 0C6ABh
                   dd 0C663h, 0C61Ah, 0C5D2h, 0C589h, 0C540h, 0C4F7h, 0C4AEh, 0C465h
                   dd 0C41Bh, 0C3D2h, 0C388h, 0C33Eh, 0C2F4h, 0C2AAh, 0C260h, 0C215h
                   dd 0C1CAh, 0C180h, 0C135h, 0C0EAh, 0C09Eh, 0C053h, 0C007h, 0BFBCh
                   dd 0BF70h, 0BF24h, 0BED8h, 0BE8Bh, 0BE3Fh, 0BDF2h, 0BDA5h, 0BD58h
                   dd 0BD0Bh, 0BCBEh, 0BC71h, 0BC23h, 0BBD6h, 0BB88h, 0BB3Ah, 0BAECh
                   dd 0BA9Eh, 0BA4Fh, 0BA01h, 0B9B2h, 0B963h, 0B914h, 0B8C5h, 0B876h
                   dd 0B827h, 0B7D7h, 0B787h, 0B738h, 0B6E8h, 0B698h, 0B647h, 0B5F7h
                   dd 0B5A6h, 0B556h, 0B505h, 0B4B4h, 0B463h, 0B412h, 0B3C0h, 0B36Fh
                   dd 0B31Dh, 0B2CBh, 0B279h, 0B227h, 0B1D5h, 0B183h, 0B130h, 0B0DEh
                   dd 0B08Bh, 0B038h, 0AFE5h, 0AF92h, 0AF3Eh, 0AEEBh, 0AE97h, 0AE44h
                   dd 0ADF0h, 0AD9Ch, 0AD48h, 0ACF3h, 0AC9Fh, 0AC4Bh, 0ABF6h, 0ABA1h
                   dd 0AB4Ch, 0AAF7h, 0AAA2h, 0AA4Dh, 0A9F7h, 0A9A1h, 0A94Ch, 0A8F6h
                   dd 0A8A0h, 0A84Ah, 0A7F3h, 0A79Dh, 0A747h, 0A6F0h, 0A699h, 0A642h
                   dd 0A5EBh, 0A594h, 0A53Dh, 0A4E5h, 0A48Eh, 0A436h, 0A3DEh, 0A386h
                   dd 0A32Eh, 0A2D6h, 0A27Eh, 0A225h, 0A1CDh, 0A174h, 0A11Bh, 0A0C2h
                   dd 0A069h, 0A010h, 9FB7h, 9F5Dh, 9F04h, 9EAAh, 9E50h, 9DF6h
                   dd 9D9Ch, 9D42h, 9CE7h, 9C8Dh, 9C32h, 9BD8h, 9B7Dh, 9B22h
                   dd 9AC7h, 9A6Ch, 9A11h, 99B5h, 995Ah, 98FEh, 98A2h, 9846h
                   dd 97EAh, 978Eh, 9732h, 96D6h, 9679h, 961Ch, 95C0h, 9563h
                   dd 9506h, 94A9h, 944Ch, 93EEh, 9391h, 9334h, 92D6h, 9278h
                   dd 921Ah, 91BCh, 915Eh, 9100h, 90A2h, 9043h, 8FE5h, 8F86h
                   dd 8F27h, 8EC8h, 8E69h, 8E0Ah, 8DABh, 8D4Ch, 8CECh, 8C8Dh
                   dd 8C2Dh, 8BCDh, 8B6Dh, 8B0Dh, 8AADh, 8A4Dh, 89EDh, 898Ch
                   dd 892Ch, 88CBh, 886Bh, 880Ah, 87A9h, 8748h, 86E7h, 8685h
                   dd 8624h, 85C2h, 8561h, 84FFh, 849Dh, 843Ch, 83DAh, 8377h
                   dd 8315h, 82B3h, 8251h, 81EEh, 818Bh, 8129h, 80C6h, 8063h
                   dd 8000h, 7F9Dh, 7F3Ah, 7ED6h, 7E73h, 7E0Fh, 7DACh, 7D48h
                   dd 7CE4h, 7C80h, 7C1Ch, 7BB8h, 7B54h, 7AF0h, 7A8Ch, 7A27h
                   dd 79C3h, 795Eh, 78F9h, 7894h, 782Fh, 77CAh, 7765h, 7700h
                   dd 769Bh, 7635h, 75D0h, 756Ah, 7504h, 749Fh, 7439h, 73D3h
                   dd 736Dh, 7307h, 72A0h, 723Ah, 71D4h, 716Dh, 7107h, 70A0h
                   dd 7039h, 6FD2h, 6F6Bh, 6F04h, 6E9Dh, 6E36h, 6DCFh, 6D67h
                   dd 6D00h, 6C98h, 6C31h, 6BC9h, 6B61h, 6AF9h, 6A91h, 6A29h
                   dd 69C1h, 6959h, 68F1h, 6888h, 6820h, 67B7h, 674Fh, 66E6h
                   dd 667Dh, 6614h, 65ABh, 6542h, 64D9h, 6470h, 6407h, 639Eh
                   dd 6334h, 62CBh, 6261h, 61F8h, 618Eh, 6124h, 60BAh, 6050h
                   dd 5FE6h, 5F7Ch, 5F12h, 5EA8h, 5E3Dh, 5DD3h, 5D69h, 5CFEh
                   dd 5C93h, 5C29h, 5BBEh, 5B53h, 5AE8h, 5A7Dh, 5A12h, 59A7h
                   dd 593Ch, 58D1h, 5865h, 57FAh, 578Fh, 5723h, 56B8h, 564Ch
                   dd 55E0h, 5574h, 5509h, 549Dh, 5431h, 53C5h, 5358h, 52ECh
                   dd 5280h, 5214h, 51A7h, 513Bh, 50CEh, 5062h, 4FF5h, 4F88h
                   dd 4F1Ch, 4EAFh, 4E42h, 4DD5h, 4D68h, 4CFBh, 4C8Eh, 4C21h
                   dd 4BB4h, 4B46h, 4AD9h, 4A6Bh, 49FEh, 4990h, 4923h, 48B5h
                   dd 4848h, 47DAh, 476Ch, 46FEh, 4690h, 4622h, 45B4h, 4546h
                   dd 44D8h, 446Ah, 43FBh, 438Dh, 431Fh, 42B0h, 4242h, 41D3h
                   dd 4165h, 40F6h, 4088h, 4019h, 3FAAh, 3F3Bh, 3ECCh, 3E5Eh
                   dd 3DEFh, 3D80h, 3D11h, 3CA1h, 3C32h, 3BC3h, 3B54h, 3AE5h
                   dd 3A75h, 3A06h, 3996h, 3927h, 38B7h, 3848h, 37D8h, 3769h
                   dd 36F9h, 3689h, 3619h, 35AAh, 353Ah, 34CAh, 345Ah, 33EAh
                   dd 337Ah, 330Ah, 329Ah, 322Ah, 31B9h, 3149h, 30D9h, 3069h
                   dd 2FF8h, 2F88h, 2F17h, 2EA7h, 2E37h, 2DC6h, 2D55h, 2CE5h
                   dd 2C74h, 2C04h, 2B93h, 2B22h, 2AB1h, 2A41h, 29D0h, 295Fh
                   dd 28EEh, 287Dh, 280Ch, 279Bh, 272Ah, 26B9h, 2648h, 25D7h
                   dd 2566h, 24F5h, 2483h, 2412h, 23A1h, 2330h, 22BEh, 224Dh
                   dd 21DCh, 216Ah, 20F9h, 2087h, 2016h, 1FA4h, 1F33h, 1EC1h
                   dd 1E50h, 1DDEh, 1D6Dh, 1CFBh, 1C89h, 1C18h, 1BA6h, 1B34h
                   dd 1AC2h, 1A51h, 19DFh, 196Dh, 18FBh, 1889h, 1817h, 17A6h
                   dd 1734h, 16C2h, 1650h, 15DEh, 156Ch, 14FAh, 1488h, 1416h
                   dd 13A4h, 1332h, 12C0h, 124Eh, 11DCh, 1169h, 10F7h, 1085h
                   dd 1013h, 0FA1h, 0F2Fh, 0EBDh, 0E4Ah, 0DD8h, 0D66h, 0CF4h
                   dd 0C81h, 0C0Fh, 0B9Dh, 0B2Bh, 0AB8h, 0A46h, 9D4h, 961h
                   dd 8EFh, 87Dh, 80Bh, 798h, 726h, 6B4h, 641h, 5CFh
                   dd 55Ch, 4EAh, 478h, 405h, 393h, 321h, 2AEh, 23Ch
                   dd 1CAh, 157h, 0E5h, 72h, 0

; Function start: 0x0043E2D3
; void GetRLETransformTrig(int angleTenths, int *cosine, int *sine);
; Looks up sine and cosine in the quarter-wave table.
GetRLETransformTrig proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov ebx, dword ptr [ebp+8]
        and ebx, ebx
        jns L43E2F1
L43E2E1:
        add ebx, 0E10h
        js L43E2E1
        jmp L43E2F1
L43E2EB:
        sub ebx, 0E10h
L43E2F1:
        cmp ebx, 0E10h
        jg L43E2EB
        cmp ebx, 708h
        ja L43E339
        cmp ebx, 384h
        ja L43E31C
        shl ebx, 2
        mov eax, dword ptr [ebx+anRLEQuarterCosine]
        neg ebx
        mov edx, dword ptr [ebx+anRLEQuarterCosine+0E10h]
        jmp L43E37B
L43E31C:
        neg ebx
        add ebx, 708h
        shl ebx, 2
        mov eax, dword ptr [ebx+anRLEQuarterCosine]
        neg eax
        neg ebx
        mov edx, dword ptr [ebx+anRLEQuarterCosine+0E10h]
        jmp L43E37B
L43E339:
        neg ebx
        add ebx, 0E10h
        cmp ebx, 384h
        ja L43E35E
        shl ebx, 2
        mov eax, dword ptr [ebx+anRLEQuarterCosine]
        neg ebx
        mov edx, dword ptr [ebx+anRLEQuarterCosine+0E10h]
        neg edx
        jmp L43E37B
L43E35E:
        neg ebx
        add ebx, 708h
        shl ebx, 2
        mov eax, dword ptr [ebx+anRLEQuarterCosine]
        neg eax
        neg ebx
        mov edx, dword ptr [ebx+anRLEQuarterCosine+0E10h]
        neg edx
L43E37B:
        mov ebx, dword ptr [ebp+0Ch]
        mov dword ptr [ebx], eax
        mov ebx, dword ptr [ebp+10h]
        mov dword ptr [ebx], edx
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRLETransformTrig endp

; Function start: 0x0043E38B
; void CalculateRoundedRLEFixedProduct(int left, int right, int *result);
; Multiplies two fixed-point values with rounding.
CalculateRoundedRLEFixedProduct proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov eax, dword ptr [ebp+8]
        imul dword ptr [ebp+0Ch]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov edi, dword ptr [ebp+10h]
        mov dword ptr [edi], eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CalculateRoundedRLEFixedProduct endp

; Function start: 0x0043E3B1
; void TransformRLEPoint(int *point, int *result, int *origin, unsigned int
;                        angleTenths, int scaleX, int scaleY);
; Rotates and scales one point for RotateRLEImage.
TransformRLEPoint proc
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        lea eax, dword ptr [ebp-8]
        push eax
        lea eax, dword ptr [ebp-4]
        push eax
        push dword ptr [ebp+14h]
        call GetRLETransformTrig
        add esp, 0Ch
        mov esi, dword ptr [ebp+8]
        mov edi, dword ptr [ebp+10h]
        mov eax, dword ptr [esi]
        sub eax, dword ptr [edi]
        shl eax, 10h
        imul dword ptr [ebp+18h]
        add eax, 8000h
        adc edx, 0
        mov ebx, edx
        mov eax, ebx
        imul dword ptr [ebp-4]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp-0Ch], eax
        mov eax, ebx
        imul dword ptr [ebp-8]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp-14h], eax
        mov eax, dword ptr [esi+4]
        sub eax, dword ptr [edi+4]
        shl eax, 10h
        imul dword ptr [ebp+1Ch]
        add eax, 8000h
        adc edx, 0
        mov ecx, edx
        mov eax, ecx
        imul dword ptr [ebp-4]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp-18h], eax
        mov eax, ecx
        imul dword ptr [ebp-8]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp-10h], eax
        mov esi, dword ptr [ebp+0Ch]
        mov edx, dword ptr [ebp-0Ch]
        sub edx, dword ptr [ebp-10h]
        add edx, dword ptr [edi]
        mov dword ptr [esi], edx
        mov edx, dword ptr [ebp-18h]
        add edx, dword ptr [ebp-14h]
        add edx, dword ptr [edi+4]
        mov dword ptr [esi+4], edx
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
TransformRLEPoint endp

; Function start: 0x0043E478
; unsigned int GetRawImageHeight(unsigned char *shape);
; Returns the height of a raw image.
GetRawImageHeight proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+8]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRawImageHeight endp

; Function start: 0x0043E48B
; unsigned int GetRawFrameWidth(unsigned char *shape, int frame);
; Returns the width of a raw image frame.
GetRawFrameWidth proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 2
        add eax, dword ptr [ebp+8]
        add eax, 10h
        mov esi, dword ptr [eax]
        add esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRawFrameWidth endp

; Function start: 0x0043E4AB
; int BlitRawFrame(RasterClip *clip, int x, int y, unsigned char *shape, int
;                  frame, unsigned char *translation);
; Draws a raw image frame through a clip.
BlitRawFrame proc
        push ebp
        mov ebp, esp
        add esp, -30h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-28h], eax
        jle L43E52A
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43E52A
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-2Ch], eax
        cmp eax, 0
        jg L43E4DE
        mov eax, 0
L43E4DE:
        mov dword ptr [ebp-14h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-30h], eax
        cmp eax, 0
        jg L43E4F1
        mov eax, 0
L43E4F1:
        mov dword ptr [ebp-18h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-28h]
        dec edx
        cmp eax, edx
        jl L43E501
        mov eax, edx
L43E501:
        mov dword ptr [ebp-1Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43E510
        mov eax, edx
L43E510:
        mov dword ptr [ebp-20h], eax
        mov eax, dword ptr [ebp-1Ch]
        cmp eax, dword ptr [ebp-14h]
        jl L43E535
        mov eax, dword ptr [ebp-20h]
        cmp eax, dword ptr [ebp-18h]
        jl L43E535
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-24h], eax
        jmp L43E540
L43E52A:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43E535:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43E540:
        mov eax, dword ptr [ebp-2Ch]
        add dword ptr [ebp+0Ch], eax
        mov eax, dword ptr [ebp-30h]
        add dword ptr [ebp+10h], eax
        mov esi, dword ptr [ebp+14h]
        mov edx, dword ptr [esi+8]
        mov eax, dword ptr [ebp+18h]
        shl eax, 2
        add eax, dword ptr [ebp+14h]
        add eax, 10h
        mov esi, dword ptr [eax]
        add esi, dword ptr [ebp+14h]
        mov dword ptr [ebp-8], 0
        mov ecx, dword ptr [esi]
        mov dword ptr [ebp-4], ecx
        cmp ecx, 0
        je L43E632
        add esi, 4
        mov edi, dword ptr [ebp+8]
        mov eax, dword ptr [ebp-1Ch]
        inc eax
        sub eax, ecx
        sub eax, dword ptr [ebp+0Ch]
        jns L43E591
        add ecx, eax
        jle L43E632
L43E591:
        mov eax, dword ptr [ebp+0Ch]
        sub eax, dword ptr [ebp-14h]
        jns L43E5A6
        add ecx, eax
        jle L43E632
        sub esi, eax
        sub dword ptr [ebp+0Ch], eax
L43E5A6:
        mov eax, dword ptr [ebp-20h]
        inc eax
        sub eax, edx
        sub eax, dword ptr [ebp+10h]
        jns L43E5B5
        add edx, eax
        jle L43E632
L43E5B5:
        mov eax, dword ptr [ebp+10h]
        sub eax, dword ptr [ebp-18h]
        jns L43E5CA
        add edx, eax
        jle L43E632
        sub dword ptr [ebp+10h], eax
        imul eax, dword ptr [ebp-4]
        sub esi, eax
L43E5CA:
        mov dword ptr [ebp-10h], edx
        mov eax, dword ptr [ebp+10h]
        imul dword ptr [ebp-28h]
        add eax, dword ptr [ebp-24h]
        add eax, dword ptr [ebp+0Ch]
        mov edi, eax
        mov dword ptr [ebp-8], ecx
        sub dword ptr [ebp-4], ecx
        mov eax, dword ptr [ebp-28h]
        sub eax, ecx
        mov dword ptr [ebp-0Ch], eax
        mov edx, dword ptr [ebp-10h]
        cmp dword ptr [ebp+1Ch], 0
        jne L43E614
L43E5F2:
        shr ecx, 1
        rep movsw
        adc ecx, 0
        rep movsb
        mov ecx, dword ptr [ebp-8]
        add esi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        dec edx
        jne L43E5F2
        mov eax, dword ptr [ebp-4]
        add eax, dword ptr [ebp-8]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43E614:
        jecxz L43E632
        mov ebx, dword ptr [ebp+1Ch]
L43E619:
        mov al, byte ptr [esi]
        xlatb
        cmp al, 0FFh
        je L43E622
        mov byte ptr [edi], al
L43E622:
        inc esi
        inc edi
        loop L43E619
        mov ecx, dword ptr [ebp-8]
        add esi, dword ptr [ebp-4]
        add edi, dword ptr [ebp-0Ch]
        dec edx
        jne L43E619
L43E632:
        mov eax, dword ptr [ebp-4]
        add eax, dword ptr [ebp-8]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
BlitRawFrame endp

; Function start: 0x0043E63E
; void BlitSelectedRawFrames(RasterClip *clip, int x, int y, unsigned char
;                            *shape, const unsigned char *frames, unsigned
;                            char *translation);
; Draws several raw image frames.
BlitSelectedRawFrames proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+18h]
        mov edi, dword ptr [ebp+0Ch]
L43E64E:
        movzx eax, byte ptr [esi]
        push dword ptr [ebp+1Ch]
        push eax
        push dword ptr [ebp+14h]
        push dword ptr [ebp+10h]
        push edi
        push dword ptr [ebp+8]
        call BlitRawFrame
        add esp, 18h
        add edi, eax
        inc esi
        cmp byte ptr [esi], 0
        jne L43E64E
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
BlitSelectedRawFrames endp

; Function start: 0x0043E675
; int BlitRawScanline(RasterClip *clip, int y, const unsigned char *pixels,
;                     int width);
; Draws one raw scanline.
BlitRawScanline proc
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
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-1Ch], eax
        jle L43E6F4
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43E6F4
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-20h], eax
        cmp eax, 0
        jg L43E6A8
        mov eax, 0
L43E6A8:
        mov dword ptr [ebp-8], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-24h], eax
        cmp eax, 0
        jg L43E6BB
        mov eax, 0
L43E6BB:
        mov dword ptr [ebp-0Ch], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-1Ch]
        dec edx
        cmp eax, edx
        jl L43E6CB
        mov eax, edx
L43E6CB:
        mov dword ptr [ebp-10h], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43E6DA
        mov eax, edx
L43E6DA:
        mov dword ptr [ebp-14h], eax
        mov eax, dword ptr [ebp-10h]
        cmp eax, dword ptr [ebp-8]
        jl L43E6FF
        mov eax, dword ptr [ebp-14h]
        cmp eax, dword ptr [ebp-0Ch]
        jl L43E6FF
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-18h], eax
        jmp L43E70A
L43E6F4:
        mov eax, 0FFFFFFFFh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43E6FF:
        mov eax, 0FFFFFFFEh
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
L43E70A:
        mov dword ptr [ebp-4], 0
        mov eax, dword ptr [ebp-20h]
        add dword ptr [ebp-4], eax
        mov eax, dword ptr [ebp-24h]
        add dword ptr [ebp+0Ch], eax
        mov esi, dword ptr [ebp+10h]
        mov edi, dword ptr [ebp+8]
        mov ecx, dword ptr [ebp+14h]
        mov eax, dword ptr [ebp-10h]
        sub eax, dword ptr [ebp-4]
        inc eax
        sub eax, ecx
        jns L43E735
        add ecx, eax
        jle L43E772
L43E735:
        mov eax, dword ptr [ebp-4]
        sub eax, dword ptr [ebp-8]
        jns L43E746
        add ecx, eax
        jle L43E772
        sub esi, eax
        sub dword ptr [ebp-4], eax
L43E746:
        mov eax, dword ptr [ebp-14h]
        sub eax, dword ptr [ebp+0Ch]
        js L43E772
        mov eax, dword ptr [ebp+0Ch]
        sub eax, dword ptr [ebp-0Ch]
        js L43E772
        mov eax, dword ptr [ebp+0Ch]
        imul dword ptr [ebp-1Ch]
        add eax, dword ptr [ebp-18h]
        add eax, dword ptr [ebp-4]
        mov edi, eax
        mov edx, ecx
        and ecx, 3
        rep movsb
        mov ecx, edx
        shr ecx, 2
        rep movsd
L43E772:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
BlitRawScanline endp

; IFF chunk identifiers.
abIffBmhdChunkId db 'BMHD'
abIffCmapChunkId db 'CMAP'
abIffBodyChunkId db 'BODY'

; Function start: 0x0043E784
; unsigned char *FindIFFChunkData(const char *chunkId, const unsigned char
;                                 *iffData);
; Finds a chunk in an IFF image.
FindIFFChunkData proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+0Ch]
        add esi, 0Ch
L43E794:
        cmp byte ptr [esi], 0
        jne L43E79C
        inc esi
        jmp L43E794
L43E79C:
        mov ecx, 2
        mov edi, dword ptr [ebp+8]
        mov eax, esi
        repe cmpsw
        je L43E7BD
        mov esi, eax
        add esi, 6
        lodsw
        xchg al, ah
        and eax, 0FFFFh
        add esi, eax
        jmp L43E794
L43E7BD:
        add eax, 8
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
FindIFFChunkData endp

; Function start: 0x0043E7C6
; unsigned int DecodeIFFImage(RasterClip *clip, const unsigned char *iffData);
; Decodes an IFF ILBM image.
DecodeIFFImage proc
        push ebp
        mov ebp, esp
        add esp, -3Ch
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov edi, dword ptr [ebp+8]
        mov eax, dword ptr [edi+0Ch]
        sub eax, dword ptr [edi+4]
        inc eax
        mov dword ptr [ebp-24h], eax
        mov eax, dword ptr [edi+10h]
        sub eax, dword ptr [edi+8]
        inc eax
        mov dword ptr [ebp-20h], eax
        mov edi, dword ptr [edi]
        mov edi, dword ptr [edi]
        mov dword ptr [ebp-3Ch], edi
        mov dword ptr [ebp-28h], 0
        mov edi, dword ptr [ebp+0Ch]
        mov eax, dword ptr [edi+8]
        xor eax, 4D424C49h
        mov dword ptr [ebp-4], eax
        push dword ptr [ebp+0Ch]
        push offset abIffBmhdChunkId
        call FindIFFChunkData
        add esp, 8
        mov esi, eax
        lodsw
        xchg al, ah
        and eax, 0FFFFh
        mov dword ptr [ebp-0Ch], eax
        lodsw
        xchg al, ah
        and eax, 0FFFFh
        cmp eax, dword ptr [ebp-20h]
        jl L43E835
        mov eax, dword ptr [ebp-20h]
L43E835:
        mov dword ptr [ebp-8], eax
        add esi, 5
        lodsb
        cmp al, 1
        je L43E984
        mov eax, 0
        lodsb
        mov dword ptr [ebp-2Ch], eax
        add esi, 2
        mov eax, 0
        lodsb
        mov dword ptr [ebp-38h], eax
        mov eax, dword ptr [ebp-0Ch]
        mov ebx, eax
        shr eax, 3
        and ebx, 7
        cmp ebx, 1
        sbb eax, -1
        mov ebx, eax
        and eax, 1
        add ebx, eax
        mov dword ptr [ebp-30h], ebx
        mov eax, dword ptr [ebp-0Ch]
        and eax, 1
        add eax, dword ptr [ebp-0Ch]
        mov dword ptr [ebp-34h], eax
        mov eax, dword ptr [ebp-0Ch]
        cmp eax, dword ptr [ebp-24h]
        jl L43E88B
        mov eax, dword ptr [ebp-24h]
L43E88B:
        mov dword ptr [ebp-0Ch], eax
        push dword ptr [ebp+0Ch]
        push offset abIffBodyChunkId
        call FindIFFChunkData
        add esp, 8
        mov dword ptr [ebp-10h], eax
L43E8A1:
        mov esi, dword ptr [ebp-10h]
        cmp dword ptr [ebp-2Ch], 1
        jne L43E8FC
        mov edi, offset abRasterPaletteUsageScratch
        mov edx, dword ptr [ebp-34h]
        add edx, edi
L43E8B4:
        cmp edi, edx
        jae L43E8F2
        lodsb
        movzx ecx, al
        cmp ecx, 80h
        je L43E8B4
        ja L43E8D5
        inc ecx
        push ecx
        and ecx, 3
        rep movsb
        pop ecx
        shr ecx, 2
        rep movsd
        jmp L43E8B4
L43E8D5:
        lodsb
        mov ah, al
        mov ebx, eax
        shl eax, 10h
        mov ax, bx
        neg cl
        inc cl
        push ecx
        and ecx, 3
        rep stosb
        pop ecx
        shr ecx, 2
        rep stosd
        jmp L43E8B4
L43E8F2:
        mov dword ptr [ebp-10h], esi
        mov esi, offset abRasterPaletteUsageScratch
        jmp L43E904
L43E8FC:
        mov eax, esi
        add eax, dword ptr [ebp-34h]
        mov dword ptr [ebp-10h], eax
L43E904:
        cmp dword ptr [ebp-4], 0
        jne L43E966
        mov edi, offset abRasterDecodeBuffer
        mov eax, dword ptr [ebp-30h]
        mov dword ptr [ebp-18h], eax
        mov dword ptr [ebp-1Ch], eax
        mov eax, dword ptr [ebp-0Ch]
        mov dword ptr [ebp-14h], eax
L43E91E:
        mov edx, 80h
L43E923:
        mov ebx, 0
        mov eax, 100h
L43E92D:
        movzx ecx, byte ptr [esi+ebx]
        and ecx, edx
        je L43E937
        or al, ah
L43E937:
        add ebx, dword ptr [ebp-1Ch]
        shl ah, 1
        jne L43E92D
        stosb
        dec dword ptr [ebp-14h]
        je L43E94E
        shr dl, 1
        jne L43E923
        inc esi
        dec dword ptr [ebp-18h]
        jne L43E91E
L43E94E:
        push dword ptr [ebp-0Ch]
        push offset abRasterDecodeBuffer
        push dword ptr [ebp-28h]
        push dword ptr [ebp+8]
        call BlitRawScanline
        add esp, 10h
        jmp L43E978
L43E966:
        push dword ptr [ebp-0Ch]
        push esi
        push dword ptr [ebp-28h]
        push dword ptr [ebp+8]
        call BlitRawScanline
        add esp, 10h
L43E978:
        inc dword ptr [ebp-28h]
        dec dword ptr [ebp-8]
        jne L43E8A1
L43E984:
        mov eax, dword ptr [ebp-38h]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DecodeIFFImage endp

; Function start: 0x0043E98D
; void CopyILBMPalette(const unsigned char *iffData, unsigned char *palette);
; Copies the palette of an ILBM image.
CopyILBMPalette proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        push dword ptr [ebp+8]
        push offset abIffCmapChunkId
        call FindIFFChunkData
        add esp, 8
        mov esi, eax
        mov edi, dword ptr [ebp+0Ch]
        mov ecx, 300h
L43E9B1:
        lodsb
        shr al, 2
        stosb
        loop L43E9B1
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CopyILBMPalette endp

; Function start: 0x0043E9BE
; unsigned int GetILBMImageSize(const unsigned char *iffData);
; Returns the size of an ILBM image.
GetILBMImageSize proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        push dword ptr [ebp+8]
        push offset abIffBmhdChunkId
        call FindIFFChunkData
        add esp, 8
        mov esi, eax
        lodsw
        xchg al, ah
        shl eax, 10h
        lodsw
        xchg al, ah
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetILBMImageSize endp

; Function start: 0x0043E9EB
; unsigned int DecodePCXImage(RasterClip *clip, const unsigned char *pcxData);
; Decodes a PCX image.
DecodePCXImage proc
        push ebp
        mov ebp, esp
        add esp, -0Ch
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov dword ptr [ebp-0Ch], 0
        mov edi, dword ptr [ebp+8]
        mov edi, dword ptr [edi]
        mov edi, dword ptr [edi]
        mov esi, dword ptr [ebp+0Ch]
        movzx ebx, word ptr [esi+0Ah]
        sub bx, word ptr [esi+6]
        mov dword ptr [ebp-8], ebx
        mov ebx, 0
        movzx eax, word ptr [esi+42h]
        mov dword ptr [ebp-4], eax
        add esi, 80h
L43EA26:
        mov edi, offset abRasterDecodeBuffer
        mov edx, edi
        add edx, dword ptr [ebp-4]
L43EA30:
        lodsb
        mov ah, al
        and ah, 0C0h
        xor ah, 0C0h
        jne L43EA45
        and eax, 3Fh
        mov ecx, eax
        lodsb
        rep stosb
        jmp L43EA46
L43EA45:
        stosb
L43EA46:
        cmp edi, edx
        jl L43EA30
        push dword ptr [ebp-4]
        push offset abRasterDecodeBuffer
        push ebx
        push dword ptr [ebp+8]
        call BlitRawScanline
        add esp, 10h
        inc ebx
        cmp ebx, dword ptr [ebp-8]
        jle L43EA26
        mov eax, dword ptr [ebp-0Ch]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
DecodePCXImage endp

; Function start: 0x0043EA6D
; void CopyPCXPaletteFromFileTail(const unsigned char *fileData, unsigned int
;                                 fileSize, unsigned char *palette);
; Copies the palette stored at the end of a PCX file.
CopyPCXPaletteFromFileTail proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        add esi, dword ptr [ebp+0Ch]
        sub esi, 300h
        mov edi, dword ptr [ebp+10h]
        mov ecx, 300h
L43EA8B:
        lodsb
        shr al, 2
        stosb
        loop L43EA8B
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CopyPCXPaletteFromFileTail endp

; Function start: 0x0043EA98
; unsigned int GetPCXImageSize(const unsigned char *header);
; Returns the size of a PCX image.
GetPCXImageSize proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        mov ax, word ptr [esi+8]
        sub ax, word ptr [esi+4]
        inc ax
        shl eax, 10h
        mov ax, word ptr [esi+0Ah]
        sub ax, word ptr [esi+6]
        inc ax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetPCXImageSize endp

; Function start: 0x0043EABF
; void ResetGIFLZWDictionary(void);
; Resets the LZW dictionary of the GIF decoder.
ResetGIFLZWDictionary proc
        mov ebx, 0
        mov eax, ecx
        add eax, 2
        mov dword ptr [edi], eax
        mov eax, ecx
        shl eax, 1
        mov dword ptr [edi+4], eax
L43EAD2:
        cmp ebx, ecx
        jge L43EAF1
        mov byte ptr [ebx+edi+102Eh], bl
        mov byte ptr [ebx+edi+202Eh], bl
        mov word ptr [edi+ebx*2+302Eh], 0FFFFh
        inc ebx
        jmp L43EAD2
L43EAF1:
        cmp ebx, 1000h
        jge L43EB06
        mov word ptr [edi+ebx*2+302Eh], 0FFFEh
        inc ebx
        jmp L43EAF1
L43EB06:
        ret
ResetGIFLZWDictionary endp

; Function start: 0x0043EB07
; unsigned int ReadGIFDataSubBlockByte(void);
; Reads one byte from the GIF data sub-blocks.
ReadGIFDataSubBlockByte proc
        cmp dword ptr [edi+10h], 0
        jne L43EB16
        lodsb
        and eax, 0FFh
        mov dword ptr [edi+10h], eax
L43EB16:
        lodsb
        and eax, 0FFh
        dec dword ptr [edi+10h]
        ret
ReadGIFDataSubBlockByte endp

; Function start: 0x0043EB20
; unsigned int ReadGIFLZWCode(void);
; Reads one LZW code from a GIF stream.
ReadGIFLZWCode proc
        cmp dword ptr [edi+18h], 0
        jne L43EB35
        call ReadGIFDataSubBlockByte
        mov dword ptr [edi+14h], eax
        mov dword ptr [edi+18h], 8
L43EB35:
        mov eax, edx
        cmp dword ptr [edi+18h], eax
        jge L43EB4D
        call ReadGIFDataSubBlockByte
        mov ecx, dword ptr [edi+18h]
        shl eax, cl
        or dword ptr [edi+14h], eax
        add dword ptr [edi+18h], 8
L43EB4D:
        mov ebx, edx
        movzx eax, byte ptr [ebx+abGifLZWCodeMasks]
        mov ebx, dword ptr [edi+14h]
        and ebx, eax
        push ebx
        sub dword ptr [edi+18h], edx
        mov ecx, edx
        shr dword ptr [edi+14h], cl
        pop eax
        ret
ReadGIFLZWCode endp

; Function start: 0x0043EB66
; void AppendGIFLZWDictionaryEntry(void);
; Appends one entry to the LZW dictionary.
AppendGIFLZWDictionaryEntry proc
        push ebx
        mov ebx, dword ptr [edi]
        mov word ptr [edi+ebx*2+302Eh], cx
        pop ebx
        push ebx
        mov al, byte ptr [ebx+edi+102Eh]
        mov ebx, dword ptr [edi]
        mov byte ptr [ebx+edi+202Eh], al
        mov ebx, ecx
        mov al, byte ptr [ebx+edi+102Eh]
        mov ebx, dword ptr [edi]
        mov byte ptr [ebx+edi+102Eh], al
        pop ebx
        inc dword ptr [edi]
        mov eax, dword ptr [edi]
        cmp eax, dword ptr [edi+4]
        jne L43EBAB
        cmp dword ptr [edi+1Ch], 0Ch
        jge L43EBAB
        inc dword ptr [edi+1Ch]
        shl dword ptr [edi+4], 1
L43EBAB:
        ret
AppendGIFLZWDictionaryEntry endp

; Function start: 0x0043EBAC
; void EmitGIFDecodedPixel(void);
; Stores one decoded GIF pixel, following the interlace order.
EmitGIFDecodedPixel proc
        mov ebx, dword ptr [edi+8]
        mov byte ptr [ebx+abRasterDecodeBuffer], al
        inc dword ptr [edi+8]
        dec dword ptr [edi+20h]
        cmp dword ptr [edi+20h], 0
        jne L43EC28
        push dword ptr [edi+24h]
        push offset abRasterDecodeBuffer
        push dword ptr [edi+0Ch]
        push dword ptr [pGifRasterTarget]
        call BlitRawScanline
        add esp, 10h
        mov dword ptr [edi+8], 0
        mov eax, dword ptr [edi+24h]
        mov dword ptr [edi+20h], eax
        cmp byte ptr [edi+2Ch], 0
        je L43EC16
        movzx ebx, byte ptr [edi+2Dh]
        movzx eax, byte ptr [ebx+abGifInterlaceRowSteps]
        add dword ptr [edi+0Ch], eax
        mov eax, dword ptr [edi+0Ch]
        cmp eax, dword ptr [edi+28h]
        jl L43EC14
        inc byte ptr [edi+2Dh]
        movzx ebx, byte ptr [edi+2Dh]
        movzx eax, byte ptr [ebx+abGifInterlaceRowStarts]
        mov dword ptr [edi+0Ch], eax
L43EC14:
        jmp L43EC28
L43EC16:
        inc dword ptr [edi+0Ch]
        mov eax, dword ptr [edi+0Ch]
        cmp eax, dword ptr [edi+28h]
        jl L43EC28
        mov dword ptr [edi+0Ch], 0
L43EC28:
        ret
EmitGIFDecodedPixel endp

; Function start: 0x0043EC29
; unsigned int ExpandGIFLZWImage(RasterClip *clip, const unsigned char
;                                *gifData, void *workspace);
; Expands the LZW image data of a GIF.
ExpandGIFLZWImage proc
        push ebp
        mov ebp, esp
        add esp, -18h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov edi, dword ptr [ebp+8]
        mov dword ptr [pGifRasterTarget], edi
        mov edi, dword ptr [ebp+10h]
        mov eax, 0
        mov ecx, 2Eh
        mov edx, ecx
        and ecx, 3
        rep stosb
        mov ecx, edx
        shr ecx, 2
        rep stosd
        mov edi, dword ptr [ebp+10h]
        mov esi, dword ptr [ebp+0Ch]
        movzx eax, byte ptr [esi+0Bh]
        mov dword ptr [ebp-18h], eax
        mov al, byte ptr [esi+0Ah]
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        add esi, 0Dh
        test al, 80h
        je L43EC84
        imul ebx, ebx, 3
        add esi, ebx
L43EC84:
        movzx eax, word ptr [esi+5]
        mov dword ptr [edi+24h], eax
        movzx eax, word ptr [esi+7]
        mov dword ptr [edi+28h], eax
        movzx eax, byte ptr [esi+9]
        mov byte ptr [edi+2Ch], al
        and byte ptr [edi+2Ch], 40h
        add esi, 0Ah
        test al, 80h
        je L43ECB7
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        imul ebx, ebx, 3
        add esi, ebx
L43ECB7:
        mov dword ptr [edi+10h], 0
        lodsb
        movzx ecx, al
        mov edx, 8
        push ecx
        push edx
        mov eax, 1
        shl eax, cl
        mov dword ptr [ebp-4], eax
        inc eax
        mov dword ptr [ebp-8], eax
        inc ecx
        mov dword ptr [edi+1Ch], ecx
        mov ecx, dword ptr [ebp-4]
        call ResetGIFLZWDictionary
        mov dword ptr [ebp-10h], 0FFFFh
        mov dword ptr [ebp-14h], 0
        mov byte ptr [edi+2Dh], 0
        mov eax, dword ptr [edi+24h]
        mov dword ptr [edi+20h], eax
        mov dword ptr [edi+8], 0
        mov dword ptr [edi+0Ch], 0
        pop edx
        pop ecx
L43ED0B:
        push ecx
        push edx
        mov edx, dword ptr [edi+1Ch]
        cmp edx, 8
        jg L43ED1E
        push edx
        call ReadGIFLZWCode
        pop edx
        jmp L43ED3B
L43ED1E:
        push edx
        mov edx, 8
        call ReadGIFLZWCode
        pop edx
        push eax
        push edx
        sub edx, 8
        call ReadGIFLZWCode
        pop edx
        shl eax, 8
        pop ebx
        or eax, ebx
L43ED3B:
        mov dword ptr [ebp-0Ch], eax
        pop edx
        pop ecx
        cmp eax, dword ptr [ebp-4]
        jne L43ED63
        push ecx
        push edx
        mov ecx, dword ptr [ebp-4]
        call ResetGIFLZWDictionary
        pop edx
        pop ecx
        mov eax, ecx
        inc eax
        mov dword ptr [edi+1Ch], eax
        mov dword ptr [ebp-10h], 0FFFFh
        jmp L43EE2E
L43ED63:
        cmp eax, dword ptr [ebp-8]
        jne L43ED8F
L43ED68:
        cmp dword ptr [edi+10h], 0
        je L43ED74
        lodsb
        dec dword ptr [edi+10h]
        jmp L43ED68
L43ED74:
        lodsb
        and eax, 0FFh
        mov dword ptr [edi+10h], eax
        cmp dword ptr [edi+10h], 0
        jne L43ED68
        mov dword ptr [ebp-14h], 0FFFFh
        jmp L43EE2E
L43ED8F:
        mov ebx, dword ptr [ebp-0Ch]
        cmp word ptr [edi+ebx*2+302Eh], -2
        je L43EDB4
        cmp dword ptr [ebp-10h], 0FFFFh
        je L43EDB2
        push ecx
        push edx
        mov ecx, dword ptr [ebp-10h]
        call AppendGIFLZWDictionaryEntry
        pop edx
        pop ecx
L43EDB2:
        jmp L43EDC3
L43EDB4:
        push ecx
        push edx
        mov ebx, dword ptr [ebp-10h]
        mov ecx, dword ptr [ebp-10h]
        call AppendGIFLZWDictionaryEntry
        pop edx
        pop ecx
L43EDC3:
        push ecx
        push edx
        mov ebx, dword ptr [ebp-0Ch]
        push esi
        mov ecx, 0
        mov esi, edi
        add esi, 2Eh
L43EDD3:
        mov al, byte ptr [ebx+edi+202Eh]
        mov byte ptr [esi], al
        inc esi
        inc ecx
        movzx ebx, word ptr [edi+ebx*2+302Eh]
        cmp ebx, 0FFFFh
        jne L43EDD3
        cmp edx, 1
        jne L43EE14
L43EDF3:
        dec esi
        mov al, byte ptr [esi]
        and eax, 1
        push ecx
        call EmitGIFDecodedPixel
        pop ecx
        mov al, byte ptr [esi]
        and eax, 0FFh
        shr eax, 1
        push ecx
        call EmitGIFDecodedPixel
        pop ecx
        loop L43EDF3
        jmp L43EE25
L43EE14:
        dec esi
        mov al, byte ptr [esi]
        and eax, 0FFh
        push ecx
        call EmitGIFDecodedPixel
        pop ecx
        loop L43EE14
L43EE25:
        pop esi
        pop edx
        pop ecx
        mov eax, dword ptr [ebp-0Ch]
        mov dword ptr [ebp-10h], eax
L43EE2E:
        cmp dword ptr [ebp-14h], 0
        jne L43EE39
        jmp L43ED0B
L43EE39:
        mov eax, dword ptr [ebp-18h]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
ExpandGIFLZWImage endp

; Function start: 0x0043EE42
; void CopyGIFPalette(const unsigned char *gifData, unsigned char *palette);
; Copies the palette of a GIF image.
CopyGIFPalette proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        mov al, byte ptr [esi+0Ah]
        add esi, 0Dh
        test al, 80h
        je L43EE76
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        imul ebx, ebx, 3
        mov edi, dword ptr [ebp+0Ch]
        mov ecx, ebx
L43EE6F:
        lodsb
        shr al, 2
        stosb
        loop L43EE6F
L43EE76:
        mov al, byte ptr [esi+9]
        test al, 80h
        je L43EE9D
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        imul ebx, ebx, 3
        add esi, 0Ah
        mov edi, dword ptr [ebp+0Ch]
        mov ecx, ebx
L43EE96:
        lodsb
        shr al, 2
        stosb
        loop L43EE96
L43EE9D:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CopyGIFPalette endp

; Function start: 0x0043EEA3
; unsigned int GetGIFImageSize(const unsigned char *gifData);
; Returns the size of a GIF image.
GetGIFImageSize proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        mov al, byte ptr [esi+0Ah]
        mov cl, al
        and cl, 7
        inc cl
        mov ebx, 1
        shl ebx, cl
        add esi, 0Dh
        test al, 80h
        je L43EECA
        imul ebx, ebx, 3
        add esi, ebx
L43EECA:
        mov ax, word ptr [esi+5]
        shl eax, 10h
        mov ax, word ptr [esi+7]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetGIFImageSize endp

; Function start: 0x0043EEDB
; unsigned int GetRLEFrameDimensions(unsigned char *shape, int frame);
; Returns the dimensions of an RLE frame.
GetRLEFrameDimensions proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        add esi, 8
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 3
        add esi, eax
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRLEFrameDimensions endp

; Function start: 0x0043EEFD
; unsigned int GetRLEFrameExtents(unsigned char *shape, int frame);
; Returns the extents of an RLE frame.
GetRLEFrameExtents proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        add esi, 8
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 3
        add esi, eax
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+4]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRLEFrameExtents endp

; Function start: 0x0043EF20
; unsigned int GetRLEImageSize(unsigned char *shape, int frame);
; Returns the size of one RLE image frame.
GetRLEImageSize proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        add esi, 8
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 3
        add esi, eax
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+10h]
        sub eax, dword ptr [esi+8]
        inc eax
        mov ebx, dword ptr [esi+14h]
        sub ebx, dword ptr [esi+0Ch]
        inc ebx
        shl eax, 10h
        mov ax, bx
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRLEImageSize endp

; Function start: 0x0043EF54
; unsigned int GetRLEImageOrigin(unsigned char *shape, int frame);
; Returns the origin of one RLE image frame.
GetRLEImageOrigin proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        add esi, 8
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 3
        add esi, eax
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+8]
        shl eax, 10h
        mov ax, word ptr [esi+0Ch]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRLEImageOrigin endp

; Function start: 0x0043EF7E
; void ApplyRLEFramePalette(unsigned char *shape, int frame, unsigned char
;                           *palette);
; Applies the palette stored with an RLE frame.
ApplyRLEFramePalette proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        add esi, 8
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 3
        add esi, eax
        add esi, 4
        mov esi, dword ptr [esi]
        cmp esi, 0
        je L43EFC3
        add esi, dword ptr [ebp+8]
        lodsd
        mov ecx, eax
        mov edi, dword ptr [ebp+10h]
L43EFA9:
        lodsb
        and eax, 0FFh
        mov ebx, eax
        shl ebx, 1
        add ebx, eax
        lodsb
        mov byte ptr [ebx+edi], al
        inc ebx
        lodsw
        mov word ptr [ebx+edi], ax
        dec ecx
        jne L43EFA9
L43EFC3:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
ApplyRLEFramePalette endp

; Function start: 0x0043EFC9
; unsigned int CopyRLEFramePalette(unsigned char *shape, int frame, unsigned
;                                  char *entries);
; Copies the palette stored with an RLE frame.
CopyRLEFramePalette proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        add esi, 8
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 3
        add esi, eax
        add esi, 4
        mov esi, dword ptr [esi]
        cmp esi, 0
        jne L43EFF2
        mov eax, 0
        jmp L43F00B
L43EFF2:
        add esi, dword ptr [ebp+8]
        lodsd
        mov ebx, eax
        mov edi, dword ptr [ebp+10h]
        or edi, edi
        je L43F00B
        mov ecx, ebx
        mov eax, 0
L43F006:
        movsd
        loop L43F006
        mov eax, ebx
L43F00B:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CopyRLEFramePalette endp

; Function start: 0x0043F011
; unsigned int SetRLEFramePalette(unsigned char *shape, int frame, const
;                                 unsigned char *entries);
; Stores a palette with an RLE frame.
SetRLEFramePalette proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp+8]
        add esi, 8
        mov eax, dword ptr [ebp+0Ch]
        shl eax, 3
        add esi, eax
        add esi, 4
        mov esi, dword ptr [esi]
        cmp esi, 0
        jne L43F03A
        mov eax, 0
        jmp L43F055
L43F03A:
        add esi, dword ptr [ebp+8]
        lodsd
        mov ebx, eax
        mov edi, esi
        mov esi, dword ptr [ebp+10h]
        or esi, esi
        je L43F055
        mov ecx, ebx
        mov eax, 0
L43F050:
        movsd
        loop L43F050
        mov eax, ebx
L43F055:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
SetRLEFramePalette endp

; Function start: 0x0043F05B
; unsigned int GetRLEFrameCount(const unsigned char *shape);
; Returns the number of frames in an RLE image.
GetRLEFrameCount proc
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+4]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
GetRLEFrameCount endp

; Function start: 0x0043F06E
; int CollectUniqueRLEImageFrames(const unsigned char *shape, unsigned int
;                                 *frames);
; Lists the distinct frames of an RLE image.
CollectUniqueRLEImageFrames proc
        push ebp
        mov ebp, esp
        add esp, -4
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        mov ecx, dword ptr [esi+4]
        dec ecx
        add esi, 8
        mov dword ptr [ebp-4], esi
        mov ebx, 1
        mov edx, dword ptr [ebp+0Ch]
        cmp edx, 0
        je L43F09B
        mov dword ptr [edx], 0
        add edx, 4
L43F09B:
        cmp ecx, 0
        je L43F0C8
L43F0A0:
        add esi, 8
        mov eax, dword ptr [esi]
        mov edi, dword ptr [ebp-4]
L43F0A8:
        cmp eax, dword ptr [edi]
        je L43F0C6
        add edi, 8
        cmp edi, esi
        jl L43F0A8
        cmp edx, 0
        je L43F0C5
        mov eax, esi
        sub eax, dword ptr [ebp-4]
        shr eax, 3
        mov dword ptr [edx], eax
        add edx, 4
L43F0C5:
        inc ebx
L43F0C6:
        loop L43F0A0
L43F0C8:
        mov eax, ebx
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CollectUniqueRLEImageFrames endp

; Function start: 0x0043F0D0
; int CollectUniqueRLEPaletteFrames(const unsigned char *shape, unsigned int
;                                   *frames);
; Lists the distinct palettes of an RLE image.
CollectUniqueRLEPaletteFrames proc
        push ebp
        mov ebp, esp
        add esp, -4
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp+8]
        mov ecx, dword ptr [esi+4]
        dec ecx
        add esi, 0Ch
        mov dword ptr [ebp-4], esi
        mov ebx, 1
        mov edx, dword ptr [ebp+0Ch]
        cmp edx, 0
        je L43F0FD
        mov dword ptr [edx], 0
        add edx, 4
L43F0FD:
        cmp ecx, 0
        je L43F12A
L43F102:
        add esi, 8
        mov eax, dword ptr [esi]
        mov edi, dword ptr [ebp-4]
L43F10A:
        cmp eax, dword ptr [edi]
        je L43F128
        add edi, 8
        cmp edi, esi
        jl L43F10A
        cmp edx, 0
        je L43F127
        mov eax, esi
        sub eax, dword ptr [ebp-4]
        shr eax, 3
        mov dword ptr [edx], eax
        add edx, 4
L43F127:
        inc ebx
L43F128:
        loop L43F102
L43F12A:
        mov eax, ebx
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CollectUniqueRLEPaletteFrames endp

; Maximal-length LFSR feedback masks for DissolveRasterClip, indexed by
; register width from 2.
adwRasterDissolveTaps dd 3, 6, 0Ch, 14h, 30h, 60h, 0B8h, 110h
                      dd 240h, 500h, 0CA0h, 1B00h, 3500h, 6000h, 0B400h, 12000h
                      dd 20400h, 72000h, 90000h, 140000h, 300000h, 420000h, 0D80000h, 1200000h
                      dd 3880000h, 7200000h, 9000000h, 14000000h, 32800000h, 48000000h, 0A3000000h

; Function start: 0x0043F1AE
; Unreferenced. Copies pixels from one clip to another in the order of a
; maximal-length LFSR, a dissolve that a caller can resume from the
; returned register state.  The tap table before it holds one feedback
; mask per register width.
DissolveRasterClip proc
        push ebp
        mov ebp, esp
        add esp, -70h
        push es
        push ebx
        push esi
        push edi
        mov esi, dword ptr [ebp+8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-44h], eax
        jle L43F22A
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43F22A
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-4Ch], eax
        cmp eax, 0
        jg L43F1DE
        mov eax, 0
L43F1DE:
        mov dword ptr [ebp-34h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-50h], eax
        cmp eax, 0
        jg L43F1F1
        mov eax, 0
L43F1F1:
        mov dword ptr [ebp-38h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-44h]
        dec edx
        cmp eax, edx
        jl L43F201
        mov eax, edx
L43F201:
        mov dword ptr [ebp-3Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43F210
        mov eax, edx
L43F210:
        mov dword ptr [ebp-40h], eax
        mov eax, dword ptr [ebp-3Ch]
        cmp eax, dword ptr [ebp-34h]
        jl L43F235
        mov eax, dword ptr [ebp-40h]
        cmp eax, dword ptr [ebp-38h]
        jl L43F235
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-48h], eax
        jmp L43F240
L43F22A:
        mov eax, 0FFFFFFFFh
        pop edi
        pop esi
        pop ebx
        pop es
        leave
        ret
L43F235:
        mov eax, 0FFFFFFFEh
        pop edi
        pop esi
        pop ebx
        pop es
        leave
        ret
L43F240:
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+0Ch]
        sub eax, dword ptr [esi+4]
        inc eax
        mov dword ptr [ebp-24h], eax
        mov eax, dword ptr [esi+10h]
        sub eax, dword ptr [esi+8]
        inc eax
        mov dword ptr [ebp-28h], eax
        mov eax, dword ptr [ebp-50h]
        imul dword ptr [ebp-44h]
        add eax, dword ptr [ebp-48h]
        add eax, dword ptr [ebp-4Ch]
        mov edi, eax
        xor ebx, ebx
        mov ecx, dword ptr [ebp-28h]
L43F26A:
        mov dword ptr [ebx*4+apRasterDissolveSourceRows], edi
        inc ebx
        add edi, dword ptr [ebp-44h]
        loop L43F26A
        mov esi, dword ptr [ebp+0Ch]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx+4]
        inc eax
        mov dword ptr [ebp-68h], eax
        jle L43F2E9
        mov eax, dword ptr [ebx+8]
        inc eax
        mov ecx, eax
        jle L43F2E9
        mov eax, dword ptr [esi+4]
        mov dword ptr [ebp-6Ch], eax
        cmp eax, 0
        jg L43F29D
        mov eax, 0
L43F29D:
        mov dword ptr [ebp-54h], eax
        mov eax, dword ptr [esi+8]
        mov dword ptr [ebp-70h], eax
        cmp eax, 0
        jg L43F2B0
        mov eax, 0
L43F2B0:
        mov dword ptr [ebp-58h], eax
        mov eax, dword ptr [esi+0Ch]
        mov edx, dword ptr [ebp-68h]
        dec edx
        cmp eax, edx
        jl L43F2C0
        mov eax, edx
L43F2C0:
        mov dword ptr [ebp-5Ch], eax
        mov eax, dword ptr [esi+10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl L43F2CF
        mov eax, edx
L43F2CF:
        mov dword ptr [ebp-60h], eax
        mov eax, dword ptr [ebp-5Ch]
        cmp eax, dword ptr [ebp-54h]
        jl L43F2F4
        mov eax, dword ptr [ebp-60h]
        cmp eax, dword ptr [ebp-58h]
        jl L43F2F4
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp-64h], eax
        jmp L43F2FF
L43F2E9:
        mov eax, 0FFFFFFFFh
        pop edi
        pop esi
        pop ebx
        pop es
        leave
        ret
L43F2F4:
        mov eax, 0FFFFFFFEh
        pop edi
        pop esi
        pop ebx
        pop es
        leave
        ret
L43F2FF:
        mov esi, dword ptr [ebp+0Ch]
        mov eax, dword ptr [esi+0Ch]
        sub eax, dword ptr [esi+4]
        inc eax
        mov dword ptr [ebp-2Ch], eax
        mov eax, dword ptr [esi+10h]
        sub eax, dword ptr [esi+8]
        inc eax
        mov dword ptr [ebp-30h], eax
        mov eax, dword ptr [ebp-70h]
        imul dword ptr [ebp-68h]
        add eax, dword ptr [ebp-64h]
        add eax, dword ptr [ebp-6Ch]
        mov edi, eax
        xor ebx, ebx
        mov ecx, dword ptr [ebp-30h]
L43F329:
        mov dword ptr [ebx*4+abRasterDecodeBuffer+700h], edi
        inc ebx
        add edi, dword ptr [ebp-68h]
        loop L43F329
        mov eax, dword ptr [ebp-28h]
        cmp eax, dword ptr [ebp-30h]
        jl L43F341
        mov eax, dword ptr [ebp-30h]
L43F341:
        mov dword ptr [ebp-4], eax
        mov eax, dword ptr [ebp-24h]
        mov ebx, dword ptr [ebp-2Ch]
        cmp ebx, eax
        jg L43F350
        mov eax, ebx
L43F350:
        mov dword ptr [ebp-8], eax
        xor ebx, ebx
        mov dword ptr [ebp-20h], ebx
        mov eax, dword ptr [ebp-4]
L43F35B:
        inc ebx
        stc
        rcl dword ptr [ebp-20h], 1
        shr eax, 1
        jne L43F35B
        mov dword ptr [ebp-1Ch], ebx
        mov eax, dword ptr [ebp-8]
L43F36A:
        inc ebx
        shr eax, 1
        jne L43F36A
        mov eax, dword ptr [ebx*4+adwRasterDissolveTaps-8]
        mov dword ptr [ebp-10h], eax
        cmp dword ptr [ebp+14h], 0
        jne L43F392
        mov dword ptr [ebp+14h], 1
        mov ebx, 0
        mov esi, 0
        jmp L43F3C0
L43F392:
        mov esi, dword ptr [ebp+14h]
        shr esi, 1
        jae L43F39C
        xor esi, dword ptr [ebp-10h]
L43F39C:
        mov dword ptr [ebp+14h], esi
        cmp esi, 1
        jne L43F3AB
        mov dword ptr [ebp+10h], 0
L43F3AB:
        mov ecx, dword ptr [ebp-1Ch]
        shr esi, cl
        cmp esi, dword ptr [ebp-8]
        jae L43F392
        mov ebx, dword ptr [ebp+14h]
        and ebx, dword ptr [ebp-20h]
        cmp ebx, dword ptr [ebp-4]
        jae L43F392
L43F3C0:
        dec dword ptr [ebp+10h]
        mov eax, esi
        add eax, dword ptr [ebp-6Ch]
        cmp eax, dword ptr [ebp-54h]
        jl L43F392
        cmp eax, dword ptr [ebp-5Ch]
        jg L43F392
        mov eax, esi
        add eax, dword ptr [ebp-4Ch]
        cmp eax, dword ptr [ebp-34h]
        jl L43F392
        cmp eax, dword ptr [ebp-3Ch]
        jg L43F392
        mov eax, ebx
        add eax, dword ptr [ebp-70h]
        cmp eax, dword ptr [ebp-58h]
        jl L43F392
        cmp eax, dword ptr [ebp-60h]
        jg L43F392
        mov eax, ebx
        add eax, dword ptr [ebp-50h]
        cmp eax, dword ptr [ebp-38h]
        jl L43F392
        cmp eax, dword ptr [ebp-40h]
        jg L43F392
        mov edi, dword ptr [ebx*4+abRasterDecodeBuffer+700h]
        add edi, esi
        mov eax, dword ptr [ebx*4+apRasterDissolveSourceRows]
        add esi, eax
        movsb
        cmp dword ptr [ebp+10h], 0
        jge L43F392
        mov eax, dword ptr [ebp+14h]
        pop edi
        pop esi
        pop ebx
        pop es
        leave
        ret
DissolveRasterClip endp

; Function start: 0x0043F425
; void FadeRasterPaletteToPalette(RasterSurface *surface, const unsigned char
;                                 *targetPalette, unsigned int duration);
; Fades a surface palette towards another in fixed point, through the
; driver callbacks.
FadeRasterPaletteToPalette proc
        push ebp
        mov ebp, esp
        add esp, -10h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov edi, offset abRasterPaletteUsageScratch
        mov eax, 0
        mov ecx, 40h
        rep stosd
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+4]
        inc eax
        mov ecx, dword ptr [esi+8]
        inc ecx
        imul ecx, eax
        mov esi, dword ptr [esi]
        mov edi, offset abRasterDecodeBuffer+300h
        mov dword ptr [ebp-4], 0FFFFFFFFh
L43F45F:
        mov eax, 0
        lodsb
        cmp byte ptr [eax+abRasterPaletteUsageScratch], 0
        jne L43F492
        or byte ptr [eax+abRasterPaletteUsageScratch], 1
        stosb
        inc dword ptr [ebp-4]
        mov ebx, eax
        shl ebx, 1
        add ebx, eax
        add ebx, offset abRasterDecodeBuffer
        push ecx
        push ebx
        push eax
        call dword ptr [apRasterDriverCallbacks+20h]
        add esp, 8
        pop ecx
L43F492:
        dec ecx
        jne L43F45F
        mov dword ptr [ebp-8], 0
        mov ecx, dword ptr [ebp-4]
        mov ebx, 0
        mov esi, dword ptr [ebp+0Ch]
L43F4A7:
        movzx eax, byte ptr [ecx+abRasterDecodeBuffer+300h]
        mov ebx, eax
        shl ebx, 1
        add ebx, eax
        mov edx, 2
L43F4B9:
        mov ah, 0FFh
        mov al, byte ptr [ebx+abRasterDecodeBuffer]
        sub al, byte ptr [ebx+esi]
        jge L43F4CA
        mov ah, 1
        neg al
L43F4CA:
        mov byte ptr [ebx+abRasterPaletteUsageScratch], ah
        mov byte ptr [ebx+abRasterDecodeBuffer+400h], al
        cmp al, byte ptr [ebp-8]
        jle L43F4DE
        mov byte ptr [ebp-8], al
L43F4DE:
        inc ebx
        dec edx
        jns L43F4B9
        dec ecx
        jns L43F4A7
        mov ecx, dword ptr [ebp-4]
        mov edi, offset abRasterFadeAccumulator
        mov al, byte ptr [ebp-8]
        shr al, 1
        mov ah, al
        shr ecx, 1
        rep stosw
        adc ecx, 0
        rep stosb
        movzx esi, byte ptr [ebp-8]
        cmp esi, 0
        je L43F5A3
        mov eax, dword ptr [ebp+10h]
        mov edx, 0
        shld edx, eax, 10h
        shl eax, 10h
        div esi
        mov dword ptr [ebp-0Ch], eax
        mov dword ptr [ebp-10h], 8000h
L43F526:
        mov ecx, dword ptr [ebp-4]
L43F529:
        movzx eax, byte ptr [ecx+abRasterDecodeBuffer+300h]
        mov ebx, eax
        shl ebx, 1
        add ebx, eax
        mov edx, 2
L43F53B:
        mov al, byte ptr [edx+ebx+abRasterFadeAccumulator]
        add al, byte ptr [edx+ebx+abRasterDecodeBuffer+400h]
        cmp al, byte ptr [ebp-8]
        jl L43F55F
        sub al, byte ptr [ebp-8]
        mov ah, byte ptr [edx+ebx+abRasterPaletteUsageScratch]
        add byte ptr [edx+ebx+abRasterDecodeBuffer], ah
L43F55F:
        mov byte ptr [edx+ebx+abRasterFadeAccumulator], al
        dec edx
        jns L43F53B
        push ecx
        mov eax, ebx
        add eax, offset abRasterDecodeBuffer
        push eax
        movzx eax, byte ptr [ecx+abRasterDecodeBuffer+300h]
        push eax
        call dword ptr [apRasterDriverCallbacks+24h]
        add esp, 8
        pop ecx
        dec ecx
        jns L43F529
        mov eax, dword ptr [ebp-0Ch]
        add dword ptr [ebp-10h], eax
        cmp word ptr [ebp-0Eh], 1
        jl L43F5A0
L43F594:
        call dword ptr [apRasterDriverCallbacks+14h]
        dec word ptr [ebp-0Eh]
        jne L43F594
L43F5A0:
        dec esi
        jne L43F526
L43F5A3:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
FadeRasterPaletteToPalette endp

; Function start: 0x0043F5A9
; int CollectRasterClipColours(RasterClip *clip, unsigned int *colours);
; Unreferenced. Collects the set of colours used in a clip.
CollectRasterClipColours proc
        push ebp
        mov ebp, esp
        add esp, -0Ch
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov edi, offset abRasterPaletteUsageScratch
        mov eax, 0
        mov ecx, 40h
        rep stosd
        mov esi, dword ptr [ebp+8]
        mov ecx, dword ptr [esi+0Ch]
        sub ecx, dword ptr [esi+4]
        mov dword ptr [ebp-8], ecx
        mov ebx, dword ptr [esi+10h]
        sub ebx, dword ptr [esi+8]
        mov esi, dword ptr [esi]
        mov eax, dword ptr [esi+4]
        inc eax
        mov dword ptr [ebp-0Ch], eax
        mov esi, dword ptr [ebp+8]
        mov eax, dword ptr [esi+8]
        imul dword ptr [ebp-0Ch]
        add eax, dword ptr [esi+4]
        mov esi, dword ptr [esi]
        mov esi, dword ptr [esi]
        add esi, eax
        mov edi, dword ptr [ebp+0Ch]
        mov dword ptr [ebp-4], 0FFFFFFFFh
L43F5FE:
        mov eax, 0
        movzx eax, byte ptr [ecx+esi]
        cmp byte ptr [eax+abRasterPaletteUsageScratch], 0
        jne L43F620
        or byte ptr [eax+abRasterPaletteUsageScratch], 1
        inc dword ptr [ebp-4]
        cmp edi, 0
        je L43F620
        stosd
L43F620:
        dec ecx
        jns L43F5FE
        mov ecx, dword ptr [ebp-8]
        add esi, dword ptr [ebp-0Ch]
        dec ebx
        jns L43F5FE
        mov eax, dword ptr [ebp-4]
        inc eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
CollectRasterClipColours endp

        .data

apRasterDriverCallbacks dd 0Dh dup (0)
szRasterDriverName db 'MCGA.DLL', 5 dup (0)
pRLEEncodeBuffer dd 0
nRLEPendingSkip dd 0
pRLEScanlineStart dd 0
pRLEScanCursor dd 0
pRLEOutputCursor dd 0
pRLELiteralStart dd 0
; Unreferenced.
        db 10h dup (0)
nRLEEncodedMinimumX dd 0
nRLEEncodedMinimumY dd 0
nRLEEncodedMaximumX dd 0
nRLEEncodedMaximumY dd 0
; Source row addresses for DissolveRasterClip.
apRasterDissolveSourceRows dd 140h dup (0)
; Image decode buffer; DissolveRasterClip keeps its target row
; addresses at +700h.
abRasterDecodeBuffer db 0D00h dup (0)
abRasterPaletteUsageScratch db 300h dup (0)
abRasterFadeAccumulator db 300h dup (0)
abGifLZWCodeMasks db 0, 1, 3, 7, 0Fh, 1Fh, 3Fh, 7Fh, 0FFh
abGifInterlaceRowSteps db 8, 8, 4, 2, 0
abGifInterlaceRowStarts db 0, 4, 2, 1, 0
pGifRasterTarget dd 0
abRasterPaletteTranslation db 100h dup (0)
aRLETransformVertices dd 14h dup (0)
anRLESourceSteps dd 4 dup (0)

        end
