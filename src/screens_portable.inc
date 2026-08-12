/*
 * Portable counterparts for the recovered raster library's hand-written x86
 * routines.  This file is included only by the WC1_SDL build of screens.c;
 * the recovered high-level screen code and the original x86 implementations
 * remain in their original compilation unit.
 */

void __stdcall FillRasterBytes(void *destination, unsigned int length,
                               short value)
{
    memset(destination, (unsigned char)value, length);
}

char *CopyRasterDriverName(void *const *callbacks)
{
    const char *(*getName)(void);

    getName = (const char *(*)(void))callbacks[0];
    strcpy(g_szRasterDriverName_0046e6d0, getName());
    return g_szRasterDriverName_0046e6d0;
}

void InstallRasterDriverCallbacks(void *const *callbacks)
{
    memcpy(g_apRasterDriverCallbacks_0046e69c, callbacks,
           sizeof(g_apRasterDriverCallbacks_0046e69c));
}

unsigned int SetRasterClipPixel(RasterClip *clip, int x, int y,
                                int colour)
{
    RasterSurface *surface;
    unsigned char *pixel;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    unsigned int previous;

    surface = clip->surface;
    width = surface->maximumX + 1;
    height = surface->maximumY + 1;
    if (width <= 0 || height <= 0)
        return (unsigned int)-1;
    left = clip->left > 0 ? clip->left : 0;
    top = clip->top > 0 ? clip->top : 0;
    right = clip->right < width - 1 ? clip->right : width - 1;
    bottom = clip->bottom < height - 1 ? clip->bottom : height - 1;
    if (right < left || bottom < top)
        return (unsigned int)-2;
    x += clip->left;
    y += clip->top;
    if (x < left || x > right || y < top || y > bottom)
        return (unsigned int)-3;
    pixel = surface->pixels + y * width + x;
    previous = *pixel;
    *pixel = (unsigned char)colour;
    return previous;
}

int ReadRasterClipPixel(RasterClip *clip, int x, int y)
{
    RasterSurface *surface;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;

    surface = clip->surface;
    width = surface->maximumX + 1;
    height = surface->maximumY + 1;
    if (width <= 0 || height <= 0)
        return -1;
    left = clip->left > 0 ? clip->left : 0;
    top = clip->top > 0 ? clip->top : 0;
    right = clip->right < width - 1 ? clip->right : width - 1;
    bottom = clip->bottom < height - 1 ? clip->bottom : height - 1;
    if (right < left || bottom < top)
        return -2;
    x += clip->left;
    y += clip->top;
    if (x < left || x > right || y < top || y > bottom)
        return -3;
    return surface->pixels[y * width + x];
}

unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1,
                             int x2, int y2, int mode, int colour)
{
    RasterSurface *surface;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int deltaX;
    int deltaY;
    int stepX;
    int stepY;
    int error;
    int doubledError;
    int done;

    surface = clip->surface;
    width = surface->maximumX + 1;
    height = surface->maximumY + 1;
    if (width <= 0 || height <= 0)
        return (unsigned int)-1;
    left = clip->left > 0 ? clip->left : 0;
    top = clip->top > 0 ? clip->top : 0;
    right = clip->right < width - 1 ? clip->right : width - 1;
    bottom = clip->bottom < height - 1 ? clip->bottom : height - 1;
    if (right < left || bottom < top)
        return (unsigned int)-2;
    x1 += clip->left;
    y1 += clip->top;
    x2 += clip->left;
    y2 += clip->top;
    deltaX = x2 >= x1 ? x2 - x1 : x1 - x2;
    deltaY = y2 >= y1 ? y2 - y1 : y1 - y2;
    stepX = x1 < x2 ? 1 : -1;
    stepY = y1 < y2 ? 1 : -1;
    error = deltaX - deltaY;
    done = 0;
    while (!done) {
        if (x1 >= left && x1 <= right && y1 >= top && y1 <= bottom) {
            unsigned char *pixel;

            pixel = surface->pixels + y1 * width + x1;
            if (mode == 0)
                *pixel = (unsigned char)colour;
            else if (mode == 1)
                *pixel = g_abRasterPaletteTranslation_0046ff2c[*pixel];
        }
        if (x1 == x2 && y1 == y2) {
            done = 1;
        } else {
            doubledError = error * 2;
            if (doubledError > -deltaY) {
                error -= deltaY;
                x1 += stepX;
            }
            if (doubledError < deltaX) {
                error += deltaX;
                y1 += stepY;
            }
        }
    }
    return 0;
}

int FillRasterClipCheckerboard(RasterClip *clip, int left, int top,
                               int right, int bottom,
                               unsigned char colour)
{
    RasterSurface *surface;
    int width;
    int height;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int x;
    int y;

    surface = clip->surface;
    width = surface->maximumX + 1;
    height = surface->maximumY + 1;
    if (width <= 0 || height <= 0)
        return -1;
    clipLeft = clip->left > 0 ? clip->left : 0;
    clipTop = clip->top > 0 ? clip->top : 0;
    clipRight = clip->right < width - 1 ? clip->right : width - 1;
    clipBottom = clip->bottom < height - 1 ? clip->bottom : height - 1;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return -2;
    left += clip->left;
    top += clip->top;
    right += clip->left;
    bottom += clip->top;
    if (left < clipLeft)
        left = clipLeft;
    if (top < clipTop)
        top = clipTop;
    if (right > clipRight)
        right = clipRight;
    if (bottom > clipBottom)
        bottom = clipBottom;
    if (right < left || bottom < top)
        return -3;
    y = top;
    while (y <= bottom) {
        x = left + ((left ^ y) & 1);
        while (x <= right) {
            surface->pixels[y * width + x] = colour;
            x += 2;
        }
        y++;
    }
    return 0;
}

int DrawRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                 int x, int y)
{
    RasterSurface *surface;
    RLEFrameHeader *header;
    unsigned char *commands;
    int width;
    int height;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int row;
    int rowCount;

    surface = clip->surface;
    width = surface->maximumX + 1;
    height = surface->maximumY + 1;
    if (width <= 0 || height <= 0)
        return -1;
    clipLeft = clip->left > 0 ? clip->left : 0;
    clipTop = clip->top > 0 ? clip->top : 0;
    clipRight = clip->right < width - 1 ? clip->right : width - 1;
    clipBottom = clip->bottom < height - 1 ? clip->bottom : height - 1;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return -2;
    header = (RLEFrameHeader *)(shape + *(int *)(shape + 8 + frame * 8));
    if (header->right < header->left || header->bottom < header->top)
        return -4;
    x += clip->left;
    y += clip->top;
    commands = (unsigned char *)(header + 1);
    rowCount = header->bottom - header->top + 1;
    row = 0;
    while (row < rowCount) {
        int drawX;
        int drawY;
        int rowDone;

        drawX = x + header->left;
        drawY = y + header->top + row;
        rowDone = 0;
        while (!rowDone) {
            unsigned int command;
            unsigned int count;

            command = *commands++;
            if (command == 0) {
                rowDone = 1;
            } else if (command == 1) {
                drawX += *commands++;
            } else {
                count = command >> 1;
                if ((command & 1) != 0) {
                    while (count-- != 0) {
                        if (drawX >= clipLeft && drawX <= clipRight &&
                            drawY >= clipTop && drawY <= clipBottom)
                            surface->pixels[drawY * width + drawX] = *commands;
                        commands++;
                        drawX++;
                    }
                } else {
                    unsigned char value;

                    value = *commands++;
                    while (count-- != 0) {
                        if (drawX >= clipLeft && drawX <= clipRight &&
                            drawY >= clipTop && drawY <= clipBottom)
                            surface->pixels[drawY * width + drawX] = value;
                        drawX++;
                    }
                }
            }
        }
        row++;
    }
    return 0;
}

int DrawRLEImageUnclipped(RasterClip *clip, RLEFrameHeader *header,
                          int x, int y, int strideScratch)
{
    RasterSurface *surface;
    unsigned char *commands;
    int width;
    int row;
    int rowCount;

    (void)strideScratch;
    surface = clip->surface;
    width = surface->maximumX + 1;
    x += clip->left;
    y += clip->top;
    commands = (unsigned char *)(header + 1);
    rowCount = header->bottom - header->top + 1;
    row = 0;
    while (row < rowCount) {
        unsigned char *destination;
        int rowDone;

        destination = surface->pixels +
                      (y + header->top + row) * width + x + header->left;
        rowDone = 0;
        while (!rowDone) {
            unsigned int command;
            unsigned int count;

            command = *commands++;
            if (command == 0) {
                rowDone = 1;
            } else if (command == 1) {
                destination += *commands++;
            } else {
                count = command >> 1;
                if ((command & 1) != 0) {
                    memcpy(destination, commands, count);
                    commands += count;
                    destination += count;
                } else {
                    memset(destination, *commands++, count);
                    destination += count;
                }
            }
        }
        row++;
    }
    return 0;
}

void SetPaletteTranslationTable(const unsigned char *translation)
{
    memcpy(g_abRasterPaletteTranslation_0046ff2c, translation, 256);
}

int DrawRLEImageColor(RasterClip *clip, unsigned char *shape, int frame,
                      int x, int y)
{
    RasterSurface *surface;
    RLEFrameHeader *header;
    unsigned char *commands;
    int width;
    int height;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int row;
    int rowCount;

    surface = clip->surface;
    width = surface->maximumX + 1;
    height = surface->maximumY + 1;
    if (width <= 0 || height <= 0)
        return -1;
    clipLeft = clip->left > 0 ? clip->left : 0;
    clipTop = clip->top > 0 ? clip->top : 0;
    clipRight = clip->right < width - 1 ? clip->right : width - 1;
    clipBottom = clip->bottom < height - 1 ? clip->bottom : height - 1;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return -2;
    header = (RLEFrameHeader *)(shape + *(int *)(shape + 8 + frame * 8));
    if (header->right < header->left || header->bottom < header->top)
        return -4;
    x += clip->left;
    y += clip->top;
    commands = (unsigned char *)(header + 1);
    rowCount = header->bottom - header->top + 1;
    row = 0;
    while (row < rowCount) {
        int drawX;
        int drawY;
        int rowDone;

        drawX = x + header->left;
        drawY = y + header->top + row;
        rowDone = 0;
        while (!rowDone) {
            unsigned int command;
            unsigned int count;

            command = *commands++;
            if (command == 0) {
                rowDone = 1;
            } else if (command == 1) {
                drawX += *commands++;
            } else {
                count = command >> 1;
                if ((command & 1) != 0) {
                    while (count-- != 0) {
                        unsigned char value;

                        value = g_abRasterPaletteTranslation_0046ff2c[*commands++];
                        if (drawX >= clipLeft && drawX <= clipRight &&
                            drawY >= clipTop && drawY <= clipBottom)
                            surface->pixels[drawY * width + drawX] = value;
                        drawX++;
                    }
                } else {
                    unsigned char value;

                    value = g_abRasterPaletteTranslation_0046ff2c[*commands++];
                    while (count-- != 0) {
                        if (drawX >= clipLeft && drawX <= clipRight &&
                            drawY >= clipTop && drawY <= clipBottom)
                            surface->pixels[drawY * width + drawX] = value;
                        drawX++;
                    }
                }
            }
        }
        row++;
    }
    return 0;
}

int DrawRLEImageColorUnclipped(RasterClip *clip, RLEFrameHeader *header,
                               int x, int y, int strideScratch)
{
    RasterSurface *surface;
    unsigned char *commands;
    int width;
    int row;
    int rowCount;

    (void)strideScratch;
    surface = clip->surface;
    width = surface->maximumX + 1;
    x += clip->left;
    y += clip->top;
    commands = (unsigned char *)(header + 1);
    rowCount = header->bottom - header->top + 1;
    row = 0;
    while (row < rowCount) {
        unsigned char *destination;
        int rowDone;

        destination = surface->pixels +
                      (y + header->top + row) * width + x + header->left;
        rowDone = 0;
        while (!rowDone) {
            unsigned int command;
            unsigned int count;

            command = *commands++;
            if (command == 0) {
                rowDone = 1;
            } else if (command == 1) {
                destination += *commands++;
            } else {
                count = command >> 1;
                if ((command & 1) != 0) {
                    while (count-- != 0)
                        *destination++ =
                            g_abRasterPaletteTranslation_0046ff2c[*commands++];
                } else {
                    unsigned char value;

                    value = g_abRasterPaletteTranslation_0046ff2c[*commands++];
                    memset(destination, value, count);
                    destination += count;
                }
            }
        }
        row++;
    }
    return 0;
}

void GetRLETransformTrig(int angleTenths, int *cosine, int *sine)
{
    int angle;

    angle = angleTenths % 3600;
    if (angle < 0)
        angle += 3600;
    if (angle <= 900) {
        *cosine = g_anRLEQuarterCosine_0043d4bf[angle];
        *sine = g_anRLEQuarterCosine_0043d4bf[900 - angle];
    } else if (angle <= 1800) {
        angle = 1800 - angle;
        *cosine = -g_anRLEQuarterCosine_0043d4bf[angle];
        *sine = g_anRLEQuarterCosine_0043d4bf[900 - angle];
    } else if (angle <= 2700) {
        angle -= 1800;
        *cosine = -g_anRLEQuarterCosine_0043d4bf[angle];
        *sine = -g_anRLEQuarterCosine_0043d4bf[900 - angle];
    } else {
        angle = 3600 - angle;
        *cosine = g_anRLEQuarterCosine_0043d4bf[angle];
        *sine = -g_anRLEQuarterCosine_0043d4bf[900 - angle];
    }
}

void CalculateRoundedRLEFixedProduct(int left, int right, int *result)
{
    *result = (int)(((long long)left * right + 0x8000) >> 16);
}

void TransformRLEPoint(int *point, int *result, int *origin,
                       unsigned int angleTenths, int scaleX, int scaleY)
{
    long long scaledX;
    long long scaledY;
    int cosine;
    int sine;

    GetRLETransformTrig((int)angleTenths, &cosine, &sine);
    scaledX = ((long long)(point[0] - origin[0]) * scaleX + 0x8000) >> 16;
    scaledY = ((long long)(point[1] - origin[1]) * scaleY + 0x8000) >> 16;
    result[0] = origin[0] +
                (int)((scaledX * cosine - scaledY * sine + 0x8000) >> 16);
    result[1] = origin[1] +
                (int)((scaledX * sine + scaledY * cosine + 0x8000) >> 16);
}

int RotateRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                   int x, int y, unsigned char *scratch,
                   unsigned int angleTenths, int scaleX, int scaleY,
                   unsigned int flags)
{
    RasterSurface scratchSurface;
    RasterClip scratchClip;
    RasterSurface *surface;
    RLEFrameHeader *header;
    int cosine;
    int sine;
    int sourceWidth;
    int sourceHeight;
    int destinationWidth;
    int destinationHeight;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int minimumX;
    int maximumX;
    int minimumY;
    int maximumY;
    int corner;
    int destinationX;
    int destinationY;

    if (scaleX == 0x10000 && scaleY == 0x10000 && angleTenths == 0) {
        if ((flags & 1) != 0)
            return DrawRLEImageColor(clip, shape, frame, x, y);
        return DrawRLEImage(clip, shape, frame, x, y);
    }
    if (scaleX == 0 || scaleY == 0)
        return 0;
    surface = clip->surface;
    destinationWidth = surface->maximumX + 1;
    destinationHeight = surface->maximumY + 1;
    if (destinationWidth <= 0 || destinationHeight <= 0)
        return -1;
    clipLeft = clip->left > 0 ? clip->left : 0;
    clipTop = clip->top > 0 ? clip->top : 0;
    clipRight = clip->right < destinationWidth - 1 ?
                    clip->right : destinationWidth - 1;
    clipBottom = clip->bottom < destinationHeight - 1 ?
                     clip->bottom : destinationHeight - 1;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return -2;
    header = (RLEFrameHeader *)(shape + *(int *)(shape + 8 + frame * 8));
    sourceWidth = header->right - header->left + 1;
    sourceHeight = header->bottom - header->top + 1;
    if (sourceWidth <= 0 || sourceHeight <= 0)
        return -4;
    if ((unsigned int)(sourceWidth * sourceHeight) > 0xfa00U)
        return -4;
    scratchSurface.pixels = scratch;
    scratchSurface.maximumX = sourceWidth - 1;
    scratchSurface.maximumY = sourceHeight - 1;
    scratchSurface.field_C = 0;
    scratchSurface.field_10 = 0;
    scratchClip.surface = &scratchSurface;
    scratchClip.left = 0;
    scratchClip.top = 0;
    scratchClip.right = sourceWidth - 1;
    scratchClip.bottom = sourceHeight - 1;
    if ((flags & 2) == 0) {
        memset(scratch, 0xff, (unsigned int)(sourceWidth * sourceHeight));
        if ((flags & 1) != 0)
            DrawRLEImageColor(&scratchClip, shape, frame,
                              -header->left, -header->top);
        else
            DrawRLEImage(&scratchClip, shape, frame,
                         -header->left, -header->top);
    }
    GetRLETransformTrig((int)angleTenths, &cosine, &sine);
    minimumX = 0x7fffffff;
    minimumY = 0x7fffffff;
    maximumX = (-0x7fffffff - 1);
    maximumY = (-0x7fffffff - 1);
    corner = 0;
    while (corner < 4) {
        int sourceX;
        int sourceY;
        int transformedX;
        int transformedY;
        long long scaledX;
        long long scaledY;

        sourceX = (corner & 1) != 0 ? header->right : header->left;
        sourceY = (corner & 2) != 0 ? header->bottom : header->top;
        scaledX = (long long)sourceX * scaleX;
        scaledY = (long long)sourceY * scaleY;
        transformedX = x +
            (int)((scaledX * cosine - scaledY * sine + 0x80000000LL) >> 32);
        transformedY = y +
            (int)((scaledX * sine + scaledY * cosine + 0x80000000LL) >> 32);
        if (transformedX < minimumX)
            minimumX = transformedX;
        if (transformedX > maximumX)
            maximumX = transformedX;
        if (transformedY < minimumY)
            minimumY = transformedY;
        if (transformedY > maximumY)
            maximumY = transformedY;
        corner++;
    }
    minimumX += clip->left;
    maximumX += clip->left;
    minimumY += clip->top;
    maximumY += clip->top;
    if (minimumX < clipLeft)
        minimumX = clipLeft;
    if (maximumX > clipRight)
        maximumX = clipRight;
    if (minimumY < clipTop)
        minimumY = clipTop;
    if (maximumY > clipBottom)
        maximumY = clipBottom;
    destinationY = minimumY;
    while (destinationY <= maximumY) {
        destinationX = minimumX;
        while (destinationX <= maximumX) {
            long long relativeX;
            long long relativeY;
            long long sourceFixedX;
            long long sourceFixedY;
            int sourceX;
            int sourceY;
            unsigned char pixel;

            relativeX = destinationX - clip->left - x;
            relativeY = destinationY - clip->top - y;
            sourceFixedX =
                (relativeX * cosine + relativeY * sine) * 0x10000;
            sourceFixedY =
                (-relativeX * sine + relativeY * cosine) * 0x10000;
            sourceX = (int)(sourceFixedX / scaleX + 0x8000) >> 16;
            sourceY = (int)(sourceFixedY / scaleY + 0x8000) >> 16;
            sourceX -= header->left;
            sourceY -= header->top;
            if (sourceX >= 0 && sourceX < sourceWidth &&
                sourceY >= 0 && sourceY < sourceHeight) {
                pixel = scratch[sourceY * sourceWidth + sourceX];
                if (pixel != 0xff)
                    surface->pixels[destinationY * destinationWidth +
                                    destinationX] = pixel;
            }
            destinationX++;
        }
        destinationY++;
    }
    return 0;
}

int FillRasterClip(RasterClip *clip, int colour)
{
    RasterSurface *surface;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int row;

    surface = clip->surface;
    width = surface->maximumX + 1;
    height = surface->maximumY + 1;
    if (width <= 0 || height <= 0)
        return -1;
    left = clip->left > 0 ? clip->left : 0;
    top = clip->top > 0 ? clip->top : 0;
    right = clip->right < width - 1 ? clip->right : width - 1;
    bottom = clip->bottom < height - 1 ? clip->bottom : height - 1;
    if (right < left || bottom < top)
        return -2;
    row = top;
    while (row <= bottom) {
        memset(surface->pixels + row * width + left,
               (unsigned char)colour, (unsigned int)(right - left + 1));
        row++;
    }
    return 0;
}

int BlitRasterClip(RasterClip *source, int sourceX, int sourceY,
                   RasterClip *destination, int destinationX,
                   int destinationY, unsigned int colour)
{
    RasterSurface *sourceSurface;
    RasterSurface *destinationSurface;
    int sourceWidth;
    int sourceHeight;
    int destinationWidth;
    int destinationHeight;
    int sourceLeft;
    int sourceTop;
    int sourceRight;
    int sourceBottom;
    int destinationLeft;
    int destinationTop;
    int destinationRight;
    int destinationBottom;
    int copyWidth;
    int copyHeight;
    int row;

    sourceSurface = source->surface;
    destinationSurface = destination->surface;
    sourceWidth = sourceSurface->maximumX + 1;
    sourceHeight = sourceSurface->maximumY + 1;
    destinationWidth = destinationSurface->maximumX + 1;
    destinationHeight = destinationSurface->maximumY + 1;
    if (sourceWidth <= 0 || sourceHeight <= 0)
        return -1;
    if (destinationWidth <= 0 || destinationHeight <= 0)
        return -2;
    sourceLeft = source->left > 0 ? source->left : 0;
    sourceTop = source->top > 0 ? source->top : 0;
    sourceRight = source->right < sourceWidth - 1 ?
                      source->right : sourceWidth - 1;
    sourceBottom = source->bottom < sourceHeight - 1 ?
                       source->bottom : sourceHeight - 1;
    destinationLeft = destination->left > 0 ? destination->left : 0;
    destinationTop = destination->top > 0 ? destination->top : 0;
    destinationRight = destination->right < destinationWidth - 1 ?
                           destination->right : destinationWidth - 1;
    destinationBottom = destination->bottom < destinationHeight - 1 ?
                            destination->bottom : destinationHeight - 1;
    sourceX += source->left;
    sourceY += source->top;
    destinationX += destination->left;
    destinationY += destination->top;
    if (sourceX < sourceLeft) {
        destinationX += sourceLeft - sourceX;
        sourceX = sourceLeft;
    }
    if (sourceY < sourceTop) {
        destinationY += sourceTop - sourceY;
        sourceY = sourceTop;
    }
    if (destinationX < destinationLeft) {
        sourceX += destinationLeft - destinationX;
        destinationX = destinationLeft;
    }
    if (destinationY < destinationTop) {
        sourceY += destinationTop - destinationY;
        destinationY = destinationTop;
    }
    copyWidth = sourceRight - sourceX + 1;
    if (destinationRight - destinationX + 1 < copyWidth)
        copyWidth = destinationRight - destinationX + 1;
    copyHeight = sourceBottom - sourceY + 1;
    if (destinationBottom - destinationY + 1 < copyHeight)
        copyHeight = destinationBottom - destinationY + 1;
    if (copyWidth <= 0 || copyHeight <= 0)
        return -3;
    if (sourceSurface->pixels == destinationSurface->pixels &&
        destinationY > sourceY) {
        row = copyHeight - 1;
        while (row >= 0) {
            unsigned char *target;

            target = destinationSurface->pixels +
                     (destinationY + row) * destinationWidth + destinationX;
            if ((colour & 0xffffff00U) != 0)
                memmove(target,
                        sourceSurface->pixels +
                            (sourceY + row) * sourceWidth + sourceX,
                        (unsigned int)copyWidth);
            else
                memset(target, (unsigned char)colour,
                       (unsigned int)copyWidth);
            row--;
        }
    } else {
        row = 0;
        while (row < copyHeight) {
            unsigned char *target;

            target = destinationSurface->pixels +
                     (destinationY + row) * destinationWidth + destinationX;
            if ((colour & 0xffffff00U) != 0)
                memmove(target,
                        sourceSurface->pixels +
                            (sourceY + row) * sourceWidth + sourceX,
                        (unsigned int)copyWidth);
            else
                memset(target, (unsigned char)colour,
                       (unsigned int)copyWidth);
            row++;
        }
    }
    return 0;
}

unsigned int DrawRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius,
                               int verticalRadius, int colour)
{
    long long radiusX2;
    long long radiusY2;
    long long twoRadiusX2;
    long long twoRadiusY2;
    long long error;
    long long changeX;
    long long changeY;
    long long stoppingX;
    long long stoppingY;
    int offsetX;
    int offsetY;

    if (horizontalRadius < 0 || verticalRadius < 0)
        return (unsigned int)-3;
    radiusX2 = (long long)horizontalRadius * horizontalRadius;
    radiusY2 = (long long)verticalRadius * verticalRadius;
    twoRadiusX2 = radiusX2 * 2;
    twoRadiusY2 = radiusY2 * 2;
    offsetX = horizontalRadius;
    offsetY = 0;
    changeX = radiusY2 * (1 - horizontalRadius * 2);
    changeY = radiusX2;
    error = 0;
    stoppingX = twoRadiusY2 * horizontalRadius;
    stoppingY = 0;
    while (stoppingX >= stoppingY) {
        SetRasterClipPixel(clip, x + offsetX, y + offsetY, colour);
        SetRasterClipPixel(clip, x - offsetX, y + offsetY, colour);
        SetRasterClipPixel(clip, x - offsetX, y - offsetY, colour);
        SetRasterClipPixel(clip, x + offsetX, y - offsetY, colour);
        offsetY++;
        stoppingY += twoRadiusX2;
        error += changeY;
        changeY += twoRadiusX2;
        if ((error * 2 + changeX) > 0) {
            offsetX--;
            stoppingX -= twoRadiusY2;
            error += changeX;
            changeX += twoRadiusY2;
        }
    }
    offsetX = 0;
    offsetY = verticalRadius;
    changeX = radiusY2;
    changeY = radiusX2 * (1 - verticalRadius * 2);
    error = 0;
    stoppingX = 0;
    stoppingY = twoRadiusX2 * verticalRadius;
    while (stoppingX <= stoppingY) {
        SetRasterClipPixel(clip, x + offsetX, y + offsetY, colour);
        SetRasterClipPixel(clip, x - offsetX, y + offsetY, colour);
        SetRasterClipPixel(clip, x - offsetX, y - offsetY, colour);
        SetRasterClipPixel(clip, x + offsetX, y - offsetY, colour);
        offsetX++;
        stoppingX += twoRadiusY2;
        error += changeX;
        changeX += twoRadiusY2;
        if ((error * 2 + changeY) > 0) {
            offsetY--;
            stoppingY -= twoRadiusX2;
            error += changeY;
            changeY += twoRadiusX2;
        }
    }
    return 0;
}

unsigned int FillRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius,
                               int verticalRadius, int colour)
{
    int row;

    if (horizontalRadius < 0 || verticalRadius < 0)
        return (unsigned int)-3;
    row = -verticalRadius;
    while (row <= verticalRadius) {
        long long numerator;
        long long denominator;
        int extent;

        numerator = (long long)horizontalRadius * horizontalRadius *
                    ((long long)verticalRadius * verticalRadius -
                     (long long)row * row);
        denominator = (long long)verticalRadius * verticalRadius;
        extent = horizontalRadius;
        if (denominator != 0) {
            while ((long long)extent * extent * denominator > numerator)
                extent--;
        }
        DrawClippedLine(clip, x - extent, y + row,
                        x + extent, y + row, 0, colour);
        row++;
    }
    return 0;
}

unsigned int GetRLEFrameDimensions(unsigned char *shape, int frame)
{
    RLEFrameHeader *header;
    unsigned int dimensions;

    header = (RLEFrameHeader *)(shape + *(int *)(shape + 8 + frame * 8));
    memcpy(&dimensions, header, sizeof(dimensions));
    return dimensions;
}

unsigned int GetRLEFrameExtents(unsigned char *shape, int frame)
{
    RLEFrameHeader *header;
    unsigned int extents;

    header = (RLEFrameHeader *)(shape + *(int *)(shape + 8 + frame * 8));
    memcpy(&extents, &header->topExtent, sizeof(extents));
    return extents;
}

unsigned int GetRLEImageSize(unsigned char *shape, int frame)
{
    RLEFrameHeader *header;
    unsigned int width;
    unsigned int height;

    header = (RLEFrameHeader *)(shape + *(int *)(shape + 8 + frame * 8));
    width = (unsigned int)(header->right - header->left + 1);
    height = (unsigned int)(header->bottom - header->top + 1);
    return (width << 16) | (height & 0xffff);
}

unsigned int GetRLEImageOrigin(unsigned char *shape, int frame)
{
    RLEFrameHeader *header;

    header = (RLEFrameHeader *)(shape + *(int *)(shape + 8 + frame * 8));
    return ((unsigned int)header->left << 16) |
           ((unsigned int)header->top & 0xffff);
}

void ApplyRLEFramePalette(unsigned char *shape, int frame,
                          unsigned char *palette)
{
    unsigned char *entries;
    unsigned int count;

    entries = shape + *(int *)(shape + 12 + frame * 8);
    if (entries == shape)
        return;
    count = *(unsigned int *)entries;
    entries += 4;
    while (count-- != 0) {
        unsigned int index;

        index = *entries++;
        memcpy(palette + index * 3, entries, 3);
        entries += 3;
    }
}

unsigned int CopyRLEFramePalette(unsigned char *shape, int frame,
                                 unsigned char *entries)
{
    unsigned char *source;
    unsigned int count;

    source = shape + *(int *)(shape + 12 + frame * 8);
    if (source == shape)
        return 0;
    count = *(unsigned int *)source;
    if (entries != 0)
        memcpy(entries, source + 4, count * 4);
    return count;
}

unsigned int SetRLEFramePalette(unsigned char *shape, int frame,
                                const unsigned char *entries)
{
    unsigned char *destination;
    unsigned int count;

    destination = shape + *(int *)(shape + 12 + frame * 8);
    if (destination == shape)
        return 0;
    count = *(unsigned int *)destination;
    if (entries != 0)
        memcpy(destination + 4, entries, count * 4);
    return count;
}

unsigned int GetRLEFrameCount(const unsigned char *shape)
{
    return *(const unsigned int *)(shape + 4);
}
