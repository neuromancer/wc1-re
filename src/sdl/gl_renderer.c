#include "wc1.h"

#include "video_internal.h"

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <SDL_opengl.h>

#include <stddef.h>

/* The enhanced backend keeps the game framebuffer indexed at 320x200 and
   renders only recorded space objects at output resolution.  All OpenGL
   resources and frame-lifetime state are private to this unit. */

typedef void (APIENTRY *GlActiveTextureProc)(GLenum texture);
typedef void (APIENTRY *GlAttachShaderProc)(GLuint program, GLuint shader);
typedef void (APIENTRY *GlBindAttribLocationProc)(GLuint program,
                                                    GLuint index,
                                                    const GLchar *name);
typedef void (APIENTRY *GlBindBufferProc)(GLenum target, GLuint buffer);
typedef void (APIENTRY *GlBindTextureProc)(GLenum target, GLuint texture);
typedef void (APIENTRY *GlBindVertexArrayProc)(GLuint array);
typedef void (APIENTRY *GlBlendFuncProc)(GLenum source, GLenum destination);
typedef void (APIENTRY *GlBufferDataProc)(GLenum target, GLsizeiptr size,
                                            const void *data, GLenum usage);
typedef void (APIENTRY *GlClearProc)(GLbitfield mask);
typedef void (APIENTRY *GlClearColorProc)(GLfloat red, GLfloat green,
                                            GLfloat blue, GLfloat alpha);
typedef void (APIENTRY *GlCompileShaderProc)(GLuint shader);
typedef GLuint (APIENTRY *GlCreateProgramProc)(void);
typedef GLuint (APIENTRY *GlCreateShaderProc)(GLenum type);
typedef void (APIENTRY *GlDeleteBuffersProc)(GLsizei count,
                                               const GLuint *buffers);
typedef void (APIENTRY *GlDeleteProgramProc)(GLuint program);
typedef void (APIENTRY *GlDeleteShaderProc)(GLuint shader);
typedef void (APIENTRY *GlDeleteTexturesProc)(GLsizei count,
                                                const GLuint *textures);
typedef void (APIENTRY *GlDeleteVertexArraysProc)(GLsizei count,
                                                    const GLuint *arrays);
typedef void (APIENTRY *GlDisableProc)(GLenum capability);
typedef void (APIENTRY *GlDrawArraysProc)(GLenum mode, GLint first,
                                            GLsizei count);
typedef void (APIENTRY *GlEnableProc)(GLenum capability);
typedef void (APIENTRY *GlEnableVertexAttribArrayProc)(GLuint index);
typedef void (APIENTRY *GlGenBuffersProc)(GLsizei count, GLuint *buffers);
typedef void (APIENTRY *GlGenTexturesProc)(GLsizei count, GLuint *textures);
typedef void (APIENTRY *GlGenVertexArraysProc)(GLsizei count,
                                                 GLuint *arrays);
typedef void (APIENTRY *GlGetProgramInfoLogProc)(GLuint program,
                                                   GLsizei bufferSize,
                                                   GLsizei *length,
                                                   GLchar *log);
typedef void (APIENTRY *GlGetProgramivProc)(GLuint program, GLenum name,
                                              GLint *value);
typedef void (APIENTRY *GlGetShaderInfoLogProc)(GLuint shader,
                                                  GLsizei bufferSize,
                                                  GLsizei *length,
                                                  GLchar *log);
typedef void (APIENTRY *GlGetShaderivProc)(GLuint shader, GLenum name,
                                             GLint *value);
typedef void (APIENTRY *GlGetIntegervProc)(GLenum name, GLint *value);
typedef GLint (APIENTRY *GlGetUniformLocationProc)(GLuint program,
                                                     const GLchar *name);
typedef void (APIENTRY *GlLinkProgramProc)(GLuint program);
typedef void (APIENTRY *GlPixelStoreiProc)(GLenum name, GLint value);
typedef void (APIENTRY *GlShaderSourceProc)(GLuint shader, GLsizei count,
                                              const GLchar *const *source,
                                              const GLint *length);
typedef void (APIENTRY *GlTexImage2DProc)(GLenum target, GLint level,
                                            GLint internalFormat,
                                            GLsizei width, GLsizei height,
                                            GLint border, GLenum format,
                                            GLenum type, const void *pixels);
typedef void (APIENTRY *GlTexImage3DProc)(GLenum target, GLint level,
                                            GLint internalFormat,
                                            GLsizei width, GLsizei height,
                                            GLsizei depth, GLint border,
                                            GLenum format, GLenum type,
                                            const void *pixels);
typedef void (APIENTRY *GlTexParameteriProc)(GLenum target, GLenum name,
                                               GLint value);
typedef void (APIENTRY *GlTexSubImage3DProc)(
    GLenum target, GLint level, GLint xOffset, GLint yOffset, GLint zOffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *pixels);
typedef void (APIENTRY *GlUniform1iProc)(GLint location, GLint value);
typedef void (APIENTRY *GlUseProgramProc)(GLuint program);
typedef void (APIENTRY *GlVertexAttribPointerProc)(GLuint index, GLint size,
                                                     GLenum type,
                                                     GLboolean normalized,
                                                     GLsizei stride,
                                                     const void *pointer);
typedef void (APIENTRY *GlViewportProc)(GLint x, GLint y, GLsizei width,
                                          GLsizei height);

typedef struct GlFunctions {
    GlActiveTextureProc ActiveTexture;
    GlAttachShaderProc AttachShader;
    GlBindAttribLocationProc BindAttribLocation;
    GlBindBufferProc BindBuffer;
    GlBindTextureProc BindTexture;
    GlBindVertexArrayProc BindVertexArray;
    GlBlendFuncProc BlendFunc;
    GlBufferDataProc BufferData;
    GlClearProc Clear;
    GlClearColorProc ClearColor;
    GlCompileShaderProc CompileShader;
    GlCreateProgramProc CreateProgram;
    GlCreateShaderProc CreateShader;
    GlDeleteBuffersProc DeleteBuffers;
    GlDeleteProgramProc DeleteProgram;
    GlDeleteShaderProc DeleteShader;
    GlDeleteTexturesProc DeleteTextures;
    GlDeleteVertexArraysProc DeleteVertexArrays;
    GlDisableProc Disable;
    GlDrawArraysProc DrawArrays;
    GlEnableProc Enable;
    GlEnableVertexAttribArrayProc EnableVertexAttribArray;
    GlGenBuffersProc GenBuffers;
    GlGenTexturesProc GenTextures;
    GlGenVertexArraysProc GenVertexArrays;
    GlGetProgramInfoLogProc GetProgramInfoLog;
    GlGetProgramivProc GetProgramiv;
    GlGetShaderInfoLogProc GetShaderInfoLog;
    GlGetShaderivProc GetShaderiv;
    GlGetIntegervProc GetIntegerv;
    GlGetUniformLocationProc GetUniformLocation;
    GlLinkProgramProc LinkProgram;
    GlPixelStoreiProc PixelStorei;
    GlShaderSourceProc ShaderSource;
    GlTexImage2DProc TexImage2D;
    GlTexImage3DProc TexImage3D;
    GlTexParameteriProc TexParameteri;
    GlTexSubImage3DProc TexSubImage3D;
    GlUniform1iProc Uniform1i;
    GlUseProgramProc UseProgram;
    GlVertexAttribPointerProc VertexAttribPointer;
    GlViewportProc Viewport;
} GlFunctions;

typedef struct GlVertex {
    float x;
    float y;
    float u;
    float v;
    float atlasLeft;
    float atlasTop;
    float atlasWidth;
    float atlasHeight;
    float clipLeft;
    float clipTop;
    float clipRight;
    float clipBottom;
    float magnification;
    float atlasLayer;
} GlVertex;

typedef struct GlCachedFrame {
    struct Shape *shape;
    int dataOffset;
    int width;
    int height;
    unsigned short shapeTableEnd;
    short leftExtent;
    short topExtent;
    short frame;
    short atlasLayer;
    short atlasLeft;
    short atlasTop;
} GlCachedFrame;

typedef struct GlSprite {
    const GlCachedFrame *cachedFrame;
    float x;
    float y;
    short angle;
    short scale;
    short flip;
    short clipLeft;
    short clipTop;
    short clipRight;
    short clipBottom;
} GlSprite;

typedef struct GlAtlasLayer {
    int nextX;
    int nextY;
    int rowHeight;
} GlAtlasLayer;

typedef enum GlFrameState {
    GL_RENDERER_FRAME_IDLE,
    GL_RENDERER_FRAME_RECORDING,
    GL_RENDERER_FRAME_COMPLETE
} GlFrameState;

/* A full world-object list may be followed by four three-part launch doors.
   The title logo uses only three additional entries, and the simulator's
   Victory screen up to 30 fireworks and its title's letters. */
#define GL_RENDERER_RECORDED_SPRITE_CAPACITY (SPACE_OBJECT_COUNT + 12 + 30 + 16)
#define GL_RENDERER_SPRITE_VERTEX_COUNT (GL_RENDERER_RECORDED_SPRITE_CAPACITY * 6)
#define GL_RENDERER_SPRITE_ATLAS_MAX_SIZE 2048
#define GL_RENDERER_SPRITE_ATLAS_LAYER_CAPACITY 4
#define GL_RENDERER_SPRITE_ATLAS_PADDING 1
#define GL_RENDERER_CACHED_FRAME_CAPACITY 2048

static const GLchar g_vertexShaderSource[] =
    "#version 150\n"
    "in vec2 a_position;\n"
    "in vec2 a_texCoord;\n"
    "in vec4 a_atlasRect;\n"
    "in vec4 a_clip;\n"
    "in vec2 a_spriteInfo;\n"
    "out vec2 v_texCoord;\n"
    "out vec2 v_screen;\n"
    "flat out vec4 v_atlasRect;\n"
    "flat out vec4 v_clip;\n"
    "flat out vec2 v_spriteInfo;\n"
    "void main()\n"
    "{\n"
    "    vec2 position;\n"
    "    position.x = (a_position.x + 0.5) / 320.0 * 2.0 - 1.0;\n"
    "    position.y = 1.0 - (a_position.y + 0.5) / 200.0 * 2.0;\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "    v_texCoord = a_texCoord;\n"
    "    v_screen = a_position;\n"
    "    v_atlasRect = a_atlasRect;\n"
    "    v_clip = a_clip;\n"
    "    v_spriteInfo = a_spriteInfo;\n"
    "}\n";

static const GLchar g_baseFragmentShaderSource[] =
    "#version 150\n"
    "uniform sampler2D u_indices;\n"
    "uniform sampler2D u_palette;\n"
    "in vec2 v_texCoord;\n"
    "out vec4 outputColour;\n"
    "void main()\n"
    "{\n"
    "    ivec2 size = textureSize(u_indices, 0);\n"
    "    ivec2 point = ivec2(v_texCoord * vec2(size));\n"
    "    point = clamp(point, ivec2(0), size - ivec2(1));\n"
    "    int index = int(floor(\n"
    "        texelFetch(u_indices, point, 0).r * 255.0 + 0.5));\n"
    "    outputColour = texelFetch(u_palette, ivec2(index, 0), 0);\n"
    "}\n";

static const GLchar g_spriteFragmentShaderSource[] =
    "#version 150\n"
    "uniform sampler2DArray u_indices;\n"
    "uniform sampler2D u_palette;\n"
    "uniform sampler2D u_baseIndices;\n"
    "uniform sampler2D u_viewMask;\n"
    "uniform int u_clearIndex;\n"
    "in vec2 v_texCoord;\n"
    "in vec2 v_screen;\n"
    "flat in vec4 v_atlasRect;\n"
    "flat in vec4 v_clip;\n"
    "flat in vec2 v_spriteInfo;\n"
    "out vec4 outputColour;\n"
    "vec4 indexedColour(ivec2 point)\n"
    "{\n"
    "    ivec4 atlasRect = ivec4(v_atlasRect);\n"
    "    ivec2 size = atlasRect.zw;\n"
    "    if (point.x < 0 || point.y < 0 ||\n"
    "        point.x >= size.x || point.y >= size.y)\n"
    "        return vec4(0.0);\n"
    "    int index = int(floor(\n"
    "        texelFetch(u_indices,\n"
    "                   ivec3(atlasRect.xy + point, int(v_spriteInfo.y)),\n"
    "                   0).r * 255.0 + 0.5));\n"
    "    if (index == 255)\n"
    "        return vec4(0.0);\n"
    "    vec4 colour = texelFetch(u_palette, ivec2(index, 0), 0);\n"
    "    return vec4(colour.rgb, 1.0);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    if (v_screen.x < v_clip.x || v_screen.y < v_clip.y ||\n"
    "        v_screen.x > v_clip.z || v_screen.y > v_clip.w)\n"
    "        discard;\n"
    "    ivec2 logical = ivec2(floor(v_screen + vec2(0.5)));\n"
    "    if (logical.x < 0 || logical.y < 0 ||\n"
    "        logical.x >= 320 || logical.y >= 200)\n"
    "        discard;\n"
    "    if (texelFetch(u_viewMask, logical, 0).r < 0.5)\n"
    "        discard;\n"
    "    int baseIndex = int(floor(\n"
    "        texelFetch(u_baseIndices, logical, 0).r * 255.0 + 0.5));\n"
    "    if (baseIndex != u_clearIndex)\n"
    "        discard;\n"
    "    vec2 size = v_atlasRect.zw;\n"
    "    vec2 source = v_texCoord * vec2(size) - vec2(0.5);\n"
    "    ivec2 first = ivec2(floor(source));\n"
    "    vec2 fraction = fract(source);\n"
    "    float band = min(1.0, 1.0 / max(v_spriteInfo.x, 0.0001));\n"
    "    fraction = clamp(\n"
    "        (fraction - vec2(0.5)) / band + vec2(0.5), 0.0, 1.0);\n"
    "    vec4 top = mix(indexedColour(first),\n"
    "                   indexedColour(first + ivec2(1, 0)), fraction.x);\n"
    "    vec4 bottom = mix(indexedColour(first + ivec2(0, 1)),\n"
    "                      indexedColour(first + ivec2(1, 1)), fraction.x);\n"
    "    outputColour = mix(top, bottom, fraction.y);\n"
    "    if (outputColour.a <= 0.0)\n"
    "        discard;\n"
    "}\n";

/* Hi-res HUD text (issue 29): in flight, cockpit text, nav labels and HUD
   messages use FONTS.FNT's 11-pixel conversation font (section 0) at its
   native size on a 640x400 layer. While the frame still shows a recorded
   letter, the base frame shows the cell without it and the layer carries
   the hi-res letter, with the mouse cursor composited above. */
#define GL_HIRES_SCALE 2
#define GL_HIRES_WIDTH (SDL_PORT_FRAME_WIDTH * GL_HIRES_SCALE)
#define GL_HIRES_HEIGHT (SDL_PORT_FRAME_HEIGHT * GL_HIRES_SCALE)
#define GL_HIRES_GLYPH_CAPACITY 1024
/* Text drawn off screen until it is copied: HUD messages in the space
   buffer and the nav map's labels. */
#define GL_HIRES_OFFSCREEN_GLYPH_CAPACITY 512
#define GL_HIRES_CELL_WIDTH 16
#define GL_HIRES_CELL_HEIGHT 8
#define GL_HIRES_TEXT_FONT 0
/* How many presents a hidden glyph whose whole line is gone is kept. */
#define GL_HIRES_HIDDEN_FRAME_LIMIT 15

typedef struct GlHiResGlyph {
    const unsigned char *buffer;
    unsigned short stride;
    short x;
    short y;
    unsigned char width;
    unsigned char height;
    unsigned char character;
    unsigned char colour;
    unsigned char background;
    unsigned char centered;
    /* 1 when the game drew the cell's first row elsewhere. */
    unsigned char skipped;
    /* Presents in a row that have not shown the letter. */
    unsigned char misses;
    /* The present when this letter first appeared at this place. */
    unsigned int stamp;
    unsigned char before[GL_HIRES_CELL_WIDTH * GL_HIRES_CELL_HEIGHT];
    unsigned char after[GL_HIRES_CELL_WIDTH * GL_HIRES_CELL_HEIGHT];
} GlHiResGlyph;

/* The layer is an index and a coverage byte per 640x400 pixel. */
static const GLchar g_overlayFragmentShaderSource[] =
    "#version 150\n"
    "uniform sampler2D u_indices;\n"
    "uniform sampler2D u_palette;\n"
    "in vec2 v_texCoord;\n"
    "out vec4 outputColour;\n"
    "void main()\n"
    "{\n"
    "    ivec2 size = textureSize(u_indices, 0);\n"
    "    ivec2 point = ivec2(v_texCoord * vec2(size));\n"
    "    point = clamp(point, ivec2(0), size - ivec2(1));\n"
    "    vec2 texel = texelFetch(u_indices, point, 0).rg;\n"
    "    if (texel.g < 0.5)\n"
    "        discard;\n"
    "    int index = int(floor(texel.r * 255.0 + 0.5));\n"
    "    outputColour = texelFetch(u_palette, ivec2(index, 0), 0);\n"
    "}\n";

typedef struct GlRenderer {
    SDL_Window *window;
    SDL_GLContext context;
    GlFunctions gl;
    GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint baseProgram;
    GLuint spriteProgram;
    GLuint baseTexture;
    GLuint paletteTexture;
    GLuint maskTexture;
    GLuint spriteAtlasTexture;
    GLuint overlayProgram;
    GLuint overlayTexture;
    GLint overlayIndicesUniform;
    GLint overlayPaletteUniform;
    int hiResLayerVisible;
    int hiResLayerDirty;
    GLint baseIndicesUniform;
    GLint basePaletteUniform;
    GLint spriteIndicesUniform;
    GLint spritePaletteUniform;
    GLint spriteBaseUniform;
    GLint spriteMaskUniform;
    GLint spriteClearUniform;
    GlFrameState frameState;
    int hasLayerSnapshot;
    int hasLastFrame;
    int spaceLayerOffsetX;
    int spaceLayerOffsetY;
    int spriteAtlasSize;
    int spriteAtlasLayerCount;
    int atlasResetPending;
    int useSoftwareForRestOfFrame;
    unsigned char spaceClearColour;
    unsigned int spriteCount;
    unsigned char *decodeScratch;
    size_t decodeScratchCapacity;
    GlAtlasLayer atlasLayers[GL_RENDERER_SPRITE_ATLAS_LAYER_CAPACITY];
    unsigned char viewMask[SDL_PORT_FRAME_WIDTH * SDL_PORT_FRAME_HEIGHT];
    unsigned char spaceFrameBase[SDL_PORT_FRAME_WIDTH * SDL_PORT_FRAME_HEIGHT];
    unsigned char presentedFrame[SDL_PORT_FRAME_WIDTH * SDL_PORT_FRAME_HEIGHT];
    unsigned char presentedPalette[256 * 4];
    unsigned char hiResCheckFrame[SDL_PORT_FRAME_WIDTH * SDL_PORT_FRAME_HEIGHT];
    unsigned char hiResLayer[GL_HIRES_WIDTH * GL_HIRES_HEIGHT * 2];
    GlCachedFrame cachedFrames[GL_RENDERER_CACHED_FRAME_CAPACITY];
    GlSprite recordedSprites[GL_RENDERER_RECORDED_SPRITE_CAPACITY];
    GlVertex spriteVertices[GL_RENDERER_SPRITE_VERTEX_COUNT];
} GlRenderer;

static GlRenderer g_renderer;
static int g_bHiResHudFonts;
static int g_nHiResHudFlightDepth;
static unsigned char *g_pHiResFont;
static size_t g_nHiResFontSize;
static int g_bHiResFontUnavailable;
static GlHiResGlyph g_aHiResGlyphs[GL_HIRES_GLYPH_CAPACITY];
static unsigned int g_nHiResGlyphCount;
static GlHiResGlyph g_aHiResOffscreenGlyphs[GL_HIRES_OFFSCREEN_GLYPH_CAPACITY];
static unsigned int g_nHiResOffscreenGlyphCount;
static GlHiResGlyph g_stPendingHiResGlyph;
static int g_bHiResGlyphPending;
static unsigned int g_nHiResPresentCount;
static unsigned char g_abHiResPointerMask[SDL_PORT_FRAME_WIDTH *
                                          SDL_PORT_FRAME_HEIGHT];
static unsigned char g_abHiResPointerColours[SDL_PORT_FRAME_WIDTH *
                                             SDL_PORT_FRAME_HEIGHT];
static const unsigned char *g_pHiResPointerBuffer;
static int g_nHiResPointerOffsetX;
static int g_nHiResPointerOffsetY;
static int g_bHiResPointerDrawn;
static int g_bHiResPointerShown;

static void ResetRecordedSprites(void)
{
    g_renderer.spriteCount = 0;
}

static void ResetSpriteAtlasCache(void)
{
    memset(g_renderer.atlasLayers, 0, sizeof(g_renderer.atlasLayers));
    memset(g_renderer.cachedFrames, 0, sizeof(g_renderer.cachedFrames));
    g_renderer.atlasResetPending = 0;
}

static void ReleaseRendererStorage(void)
{
    free(g_renderer.decodeScratch);
    g_renderer.decodeScratch = 0;
    g_renderer.decodeScratchCapacity = 0;
    ResetRecordedSprites();
    ResetSpriteAtlasCache();
}

static void CopyBaseWithoutMouse(unsigned char *destination,
                                 const unsigned char *pixels)
{
    Viewport viewport;

    memcpy(destination, pixels, sizeof(g_renderer.spaceFrameBase));
    if (bEventMouseCursorDrawnFlag == 0 ||
        stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0 ||
        stMouseCursorState.viewport->pixels != pixels)
        return;
    viewport = *stMouseCursorState.viewport;
    viewport.pixels = destination;
    viewport.allocation = destination;
    RestoreSpriteBackground(&viewport, abCursorSaveArea,
                            nMousePointerDrawnX,
                            nMousePointerDrawnY,
                            stMouseCursorState.shape,
                            stMouseCursorState.frame);
}

static void DrawGlQuad(const GlVertex *vertices);

/* Keep a private copy, loaded through the same patched and translated packet
   path as the game's fonts, so its lifetime does not depend on scene fonts.
   PacketLoad only fills buffers from the game's own allocator. */
static int LoadHiResFont(void)
{
    const char *filename;
    unsigned char *packet;
    void *savedAllocation;
    unsigned int size;
    short savedError;

    if (g_pHiResFont != 0)
        return 1;
    if (g_bHiResFontUnavailable || pDiskFileRecords == 0)
        return 0;
    g_bHiResFontUnavailable = 1;
    filename = pDiskFileRecords[0].name;
    savedError = nPacketError;
    savedAllocation = pLastPacketAllocation;
    size = GetPacketSize(filename, GL_HIRES_TEXT_FONT);
    if (size >= 772 && size <= 65535) {
        packet = PacketLoad(filename, GL_HIRES_TEXT_FONT, 0, 0, 0);
        if (packet != 0) {
            g_pHiResFont = malloc(size);
            if (g_pHiResFont != 0) {
                memcpy(g_pHiResFont, packet, size);
                g_nHiResFontSize = size;
                g_bHiResFontUnavailable = 0;
            }
            ReleasePacketHandle(packet);
        }
    }
    pLastPacketAllocation = savedAllocation;
    nPacketError = savedError;
    return g_pHiResFont != 0;
}

/* A catalog change must discard both the cached font and recorded glyphs.
   Keep the player's option, so the next draw loads the newly selected font. */
void SdlResetHiResHudFont(void)
{
    free(g_pHiResFont);
    g_pHiResFont = 0;
    g_nHiResFontSize = 0;
    g_bHiResFontUnavailable = 0;
    g_nHiResGlyphCount = 0;
    g_nHiResOffscreenGlyphCount = 0;
    g_bHiResGlyphPending = 0;
    g_renderer.hiResLayerVisible = 0;
}

/* The hi-res font's bitmap of a character, or NULL when it has none. */
static const unsigned char *FindHiResGlyph(unsigned char character,
                                           int *width, int *height)
{
    const unsigned char *data = g_pHiResFont;
    size_t offset;
    size_t pixel;

    if (data == 0)
        return 0;
    *width = data[4 + character];
    *height = data[0] | (data[1] << 8);
    offset = (size_t)data[0x104 + character] |
             ((size_t)data[0x204 + character] << 8);
    if (*width == 0 || *height <= 0 || offset < 772 ||
        offset + (size_t)*width * (size_t)*height > g_nHiResFontSize)
        return 0;
    /* Unused code-page slots can have a width and a valid offset but no
       ink. Keep the game's glyph instead of covering it with a blank one.
       Spaces intentionally have no ink. */
    if (character == ' ' || character == 0xa0)
        return data + offset;
    for (pixel = 0; pixel < (size_t)*width * (size_t)*height; pixel++) {
        if (data[offset + pixel] != 0xff && data[offset + pixel] != data[3])
            return data + offset;
    }
    return 0;
}

void SdlSetHiResHudFonts(int enabled)
{
    g_bHiResHudFonts = enabled != 0;
    if (!g_bHiResHudFonts) {
        g_nHiResGlyphCount = 0;
        g_nHiResOffscreenGlyphCount = 0;
        g_bHiResGlyphPending = 0;
    }
}

/* RunSpaceFlight marks the flight session. GameFlow widens it to the launch
   before it, which draws the cockpit's displays, and the training simulator
   to its Get Ready, Victory and Game Over screens around it: screens outside
   keep the game's own fonts. Sessions nest. */
void SdlBeginHiResHudFlight(void)
{
    g_nHiResHudFlightDepth++;
}

void SdlEndHiResHudFlight(void)
{
    if (g_nHiResHudFlightDepth > 0)
        g_nHiResHudFlightDepth--;
    if (g_nHiResHudFlightDepth == 0) {
        g_nHiResGlyphCount = 0;
        g_nHiResOffscreenGlyphCount = 0;
        g_bHiResGlyphPending = 0;
        g_bHiResPointerDrawn = 0;
        g_bHiResPointerShown = 0;
    }
}

static int SdlHiResHudFontsActive(void)
{
    /* The FM Towns release draws its own Japanese text. */
    return g_bHiResHudFonts && g_nHiResHudFlightDepth > 0 &&
        g_renderer.context != 0 &&
        g_renderer.overlayProgram != 0 && !SdlUsingTownsData();
}

/* Called by DrawFontGlyph before it draws a HUD-font character: keeps what
   the cell showed. Only a whole cell laid out in plain rows of its buffer is
   taken. Text drawn off screen keeps its buffer's coordinates until it is
   copied. */
int SdlBeginHiResGlyph(const Viewport *view, int cursorX, int y, int width,
                       int height, unsigned char character,
                       unsigned char colour, unsigned char background,
                       int centered)
{
    GlHiResGlyph *glyph;
    const GlHiResGlyph *previous;
    const GlHiResGlyph *recorded;
    const unsigned char *screen;
    unsigned int count;
    unsigned int index;
    unsigned int kept;
    long offset;
    long stride;
    int hiResWidth;
    int hiResHeight;
    int skipped;
    int pixel;
    int row;

    g_bHiResGlyphPending = 0;
    screen = stScreenView.pixels;
    if (!SdlHiResHudFontsActive() || view == 0 || view->pixels == 0 ||
        view->rowOffsets == 0 || screen == 0 || width <= 0 ||
        height < 2 || width > GL_HIRES_CELL_WIDTH ||
        height > GL_HIRES_CELL_HEIGHT || y < 0 || y < view->top ||
        y + height - 1 > view->bottom)
        return 0;
    if (height != 6) {
        /* print_message_text copies the HUD message context, which draws
           into the space buffer, and clips it to the cockpit's 60-row
           message area. The simulator's score and bonus lines use the
           context itself. Other users of the 8-pixel font, such as nav
           descriptions and modal panels, keep the game's own text. */
        if (height != 8 || background != 0xff ||
            view->pixels != stSpaceBufferViewport.pixels ||
            (view != &stSpaceBufferViewport &&
             (!centered || view->bottom - view->top != 60 ||
              view->left + view->right != 319)))
            return 0;
        /* SetHudTextColour erases a message by drawing it again in the
           space clear colour. Those glyphs are not text. */
        if (colour == cPrimaryViewBufferColour) {
            kept = 0;
            for (index = 0; index < g_nHiResOffscreenGlyphCount; index++) {
                if (g_aHiResOffscreenGlyphs[index].height == 8)
                    continue;
                if (kept != index)
                    g_aHiResOffscreenGlyphs[kept] =
                        g_aHiResOffscreenGlyphs[index];
                kept++;
            }
            g_nHiResOffscreenGlyphCount = kept;
            return 0;
        }
    }
    if (!LoadHiResFont() ||
        FindHiResGlyph(character, &hiResWidth, &hiResHeight) == 0)
        return 0;
    /* At a viewport's top row past buffer offset 0x8000, as the VDU
       headings are, DrawFontGlyph puts the first row at the top of the
       buffer. That row is blank in the cockpit font: the cell stays where
       the rest of the letter is. */
    skipped = view->top == y && (view->rowOffsets[y] & 0x8000) != 0;
    offset = (long)view->rowOffsets[y] + cursorX;
    stride = (long)view->rowOffsets[y + 1] - view->rowOffsets[y];
    if (view->pixels == screen && stride != SDL_PORT_FRAME_WIDTH)
        return 0;
    if (offset < 0 || stride < width || stride > 0xffff ||
        offset % stride + width > stride ||
        (view->pixels == screen &&
         offset / stride + height > SDL_PORT_FRAME_HEIGHT))
        return 0;
    for (row = 1; row < height; row++) {
        if ((long)view->rowOffsets[y + row] + cursorX !=
            offset + (long)row * stride)
            return 0;
    }
    glyph = &g_stPendingHiResGlyph;
    glyph->buffer = view->pixels;
    glyph->stride = (unsigned short)stride;
    glyph->x = (short)(offset % stride);
    glyph->y = (short)(offset / stride);
    glyph->width = (unsigned char)width;
    glyph->height = (unsigned char)height;
    glyph->character = character;
    glyph->colour = colour;
    glyph->background = background;
    glyph->centered = centered != 0;
    glyph->skipped = (unsigned char)skipped;
    glyph->misses = 0;
    for (row = 0; row < height; row++)
        memcpy(glyph->before + row * width,
               view->pixels + offset + (long)row * stride, (size_t)width);
    /* Transparent text redrawn over pixels a partial redraw left in place
       would capture its own old letter as background. Reuse the clean
       background under surviving ink instead, per pixel, because such a
       redraw can cut through a glyph's cell. */
    if (background == 0xff) {
        recorded = view->pixels == screen ? g_aHiResGlyphs : g_aHiResOffscreenGlyphs;
        count = view->pixels == screen ? g_nHiResGlyphCount : g_nHiResOffscreenGlyphCount;
        for (index = 0; index < count; index++) {
            previous = &recorded[index];
            if (previous->buffer != glyph->buffer || previous->stride != glyph->stride ||
                previous->x != glyph->x || previous->y != glyph->y ||
                previous->width != width || previous->height != height ||
                previous->character != character || previous->background != 0xff)
                continue;
            for (pixel = 0; pixel < width * height; pixel++) {
                if (previous->after[pixel] != previous->before[pixel] &&
                    glyph->before[pixel] == previous->after[pixel])
                    glyph->before[pixel] = previous->before[pixel];
            }
            break;
        }
    }
    g_bHiResGlyphPending = 1;
    return 1;
}

/* Keeps a glyph in a list. Text cells tile without overlapping, so a glyph
   drawn over part of a recorded one's cell replaces it. A full list starts
   over: text still shown is recorded again when next drawn, and until then
   it simply shows in the game's own font. */
static void StoreHiResGlyph(GlHiResGlyph *glyphs, unsigned int *count,
                            unsigned int capacity, const GlHiResGlyph *glyph)
{
    unsigned int index;
    unsigned int kept;
    unsigned int slot;
    unsigned int stamp;
    int removed;

    slot = *count;
    stamp = g_nHiResPresentCount;
    removed = 0;
    for (index = 0; index < *count; index++) {
        if (glyphs[index].buffer != glyph->buffer ||
            glyphs[index].x >= glyph->x + glyph->width ||
            glyph->x >= glyphs[index].x + glyphs[index].width ||
            glyphs[index].y >= glyph->y + glyph->height ||
            glyph->y >= glyphs[index].y + glyphs[index].height)
            continue;
        if (slot == *count) {
            slot = index;
            /* The same letter drawn again keeps its age. */
            if (glyphs[index].x == glyph->x && glyphs[index].y == glyph->y &&
                glyphs[index].width == glyph->width &&
                glyphs[index].height == glyph->height &&
                glyphs[index].character == glyph->character)
                stamp = glyphs[index].stamp;
        } else {
            glyphs[index].width = 0;
            removed = 1;
        }
    }
    if (slot == *count) {
        if (*count == capacity)
            *count = slot = 0;
        (*count)++;
    }
    glyphs[slot] = *glyph;
    glyphs[slot].stamp = stamp;
    if (!removed)
        return;
    kept = 0;
    for (index = 0; index < *count; index++) {
        if (glyphs[index].width == 0)
            continue;
        if (kept != index)
            glyphs[kept] = glyphs[index];
        kept++;
    }
    *count = kept;
}

/* Whether the game's glyph drew its letter at this pixel of its cell. */
static int HiResGlyphInk(const GlHiResGlyph *glyph, int pixel)
{
    if (pixel < glyph->skipped * glyph->width)
        return 0;
    return glyph->after[pixel] !=
        (glyph->background != 0xff ? glyph->background : glyph->before[pixel]);
}

static int HiResGlyphHasInk(const GlHiResGlyph *glyph)
{
    int pixel;

    for (pixel = 0; pixel < glyph->width * glyph->height; pixel++) {
        if (HiResGlyphInk(glyph, pixel))
            return 1;
    }
    return 0;
}

/* The flight pointer, which overlay_head_up_display draws into the space
   buffer after the radio messages: the buffer's pixels it covers and their
   colours, as that buffer was last copied to the screen with this offset,
   before the HUD restores what it covered. The letters it
   covers keep matching, and BuildHiResLayer draws it back above the text,
   as CopyBaseWithoutMouse and CompositeHiResMouseCursor do for the menus'
   cursor (#37). */

void SdlNoteHiResFlightPointer(void)
{
    g_bHiResPointerDrawn = 1;
}

/* Whether the flight pointer covers pixel (x, y) of the space buffer, or of
   the screen. */
static int HiResPointerCovers(int x, int y, int screen)
{
    if (!g_bHiResPointerShown)
        return 0;
    if (screen) {
        x -= g_nHiResPointerOffsetX;
        y -= g_nHiResPointerOffsetY;
    }
    if (x < 0 || y < 0 || x >= SDL_PORT_FRAME_WIDTH ||
        y >= SDL_PORT_FRAME_HEIGHT)
        return 0;
    return g_abHiResPointerMask[y * SDL_PORT_FRAME_WIDTH + x] != 0;
}

/* Whether a cell still shows the game's letter. Only the letter's own pixels
   count, so a heading's underline, a sprite's edge or a target bracket drawn
   over the cell's background keeps it. A few of them may also have turned
   back into that background: the damage report's first erase draws its
   black leader line from the top corner of the display, through the
   heading's G (#37). Erased text loses all of them, and a letter crossed
   by anything else changes colour. Pixels under the flight pointer do not
   count; pointer is 1 when pixels is the space buffer it was drawn into, 2
   when it is the screen. A space has nothing to compare: it stays with the
   letters next to it (DropHiResSpaceRuns). */
static int HiResGlyphMatches(const GlHiResGlyph *glyph,
                             const unsigned char *pixels, long stride,
                             int x, int y, int pointer)
{
    const unsigned char *cell;
    int column;
    int erased;
    int ink;
    int pixel;
    int row;

    erased = 0;
    ink = 0;
    for (row = glyph->skipped; row < glyph->height; row++) {
        cell = pixels + (long)(y + row) * stride + x;
        for (column = 0; column < glyph->width; column++) {
            pixel = row * glyph->width + column;
            if (!HiResGlyphInk(glyph, pixel) ||
                (pointer != 0 &&
                 HiResPointerCovers(x + column, y + row, pointer == 2)))
                continue;
            ink++;
            if (cell[column] == glyph->after[pixel])
                continue;
            if (cell[column] != (glyph->background != 0xff
                                     ? glyph->background
                                     : glyph->before[pixel]))
                return 0;
            erased++;
        }
    }
    return erased * 3 <= ink;
}

/* Buffers, then rows top to bottom and left to right, so a line's glyphs
   are adjacent. */
static int CompareHiResGlyphs(const void *left, const void *right)
{
    const GlHiResGlyph *first = (const GlHiResGlyph *)left;
    const GlHiResGlyph *second = (const GlHiResGlyph *)right;

    if (first->buffer != second->buffer)
        return (uintptr_t)first->buffer < (uintptr_t)second->buffer ? -1 : 1;
    if (first->y != second->y)
        return first->y - second->y;
    return first->x - second->x;
}

/* Whether the second glyph continues the first's run of text. */
static int HiResGlyphsAdjacent(const GlHiResGlyph *first,
                               const GlHiResGlyph *second)
{
    return second->buffer == first->buffer && second->y == first->y &&
        second->height == first->height &&
        second->x == first->x + first->width;
}

/* Sorts the glyphs into runs of adjacent cells and drops the runs without a
   letter: a space only stays with the text it separates. */
static void DropHiResSpaceRuns(GlHiResGlyph *glyphs, unsigned int *count)
{
    unsigned int first;
    unsigned int last;
    unsigned int index;
    unsigned int kept;
    int ink;

    qsort(glyphs, *count, sizeof(glyphs[0]), CompareHiResGlyphs);
    kept = 0;
    for (first = 0; first < *count; first = last) {
        ink = HiResGlyphHasInk(&glyphs[first]);
        for (last = first + 1; last < *count &&
             HiResGlyphsAdjacent(&glyphs[last - 1], &glyphs[last]); last++)
            ink |= HiResGlyphHasInk(&glyphs[last]);
        if (!ink)
            continue;
        for (index = first; index < last; index++) {
            if (kept != index)
                glyphs[kept] = glyphs[index];
            kept++;
        }
    }
    *count = kept;
}

/* After the game drew the glyph: keep the cell's new pixels and replace any
   earlier glyph recorded at the same place. */
void SdlEndHiResGlyph(void)
{
    GlHiResGlyph *glyph;
    const unsigned char *cell;
    int row;

    if (!g_bHiResGlyphPending)
        return;
    g_bHiResGlyphPending = 0;
    glyph = &g_stPendingHiResGlyph;
    cell = glyph->buffer + (long)glyph->y * glyph->stride + glyph->x;
    for (row = 0; row < glyph->height; row++)
        memcpy(glyph->after + row * glyph->width,
               cell + (long)row * glyph->stride, glyph->width);
    if (glyph->buffer == stScreenView.pixels)
        StoreHiResGlyph(g_aHiResGlyphs, &g_nHiResGlyphCount,
                        GL_HIRES_GLYPH_CAPACITY, glyph);
    else
        StoreHiResGlyph(g_aHiResOffscreenGlyphs, &g_nHiResOffscreenGlyphCount,
                        GL_HIRES_OFFSCREEN_GLYPH_CAPACITY, glyph);
}

/* After an off-screen buffer was copied to the screen: the glyphs it shows
   within the copied area follow it, moved by the copy's offset. One it no
   longer shows there, such as a radio message's letter under the flight
   pointer, which is drawn into the space buffer after the text, follows it
   hidden, keeping its place and age, until it has been gone for
   GL_HIRES_HIDDEN_FRAME_LIMIT copies (#37). Forgotten sooner, it came back
   as new text and the stale-run rule dropped its line's other hidden
   letters, so the line split and slid. */
static void CopyHiResGlyphsToScreen(const Viewport *source, int left, int top,
                                    int right, int bottom, int offsetX,
                                    int offsetY)
{
    const unsigned char *screen;
    GlHiResGlyph copied;
    const GlHiResGlyph *glyph;
    long stride;
    unsigned int index;
    unsigned int kept;
    int inside;
    int x;
    int y;

    screen = stScreenView.pixels;
    if (!SdlHiResHudFontsActive() || source == 0 || screen == 0 ||
        source->pixels == 0 || source->pixels == screen ||
        source->rowOffsets == 0 || source->top < 0 ||
        source->top >= source->bottom || g_nHiResOffscreenGlyphCount == 0)
        return;
    stride = (long)source->rowOffsets[source->top + 1] -
             source->rowOffsets[source->top];
    kept = 0;
    for (index = 0; index < g_nHiResOffscreenGlyphCount; index++) {
        glyph = &g_aHiResOffscreenGlyphs[index];
        if (glyph->buffer == source->pixels && glyph->stride == stride &&
            glyph->x >= left && glyph->y >= top &&
            glyph->x + glyph->width - 1 <= right &&
            glyph->y + glyph->height - 1 <= bottom) {
            if (HiResGlyphMatches(glyph, source->pixels, stride, glyph->x,
                                  glyph->y,
                                  g_bHiResPointerShown &&
                                  source->pixels == g_pHiResPointerBuffer))
                g_aHiResOffscreenGlyphs[index].misses = 0;
            else if (++g_aHiResOffscreenGlyphs[index].misses >
                     GL_HIRES_HIDDEN_FRAME_LIMIT)
                continue;
        }
        if (kept != index)
            g_aHiResOffscreenGlyphs[kept] = *glyph;
        kept++;
    }
    g_nHiResOffscreenGlyphCount = kept;
    DropHiResSpaceRuns(g_aHiResOffscreenGlyphs, &g_nHiResOffscreenGlyphCount);
    for (index = 0; index < g_nHiResOffscreenGlyphCount; index++) {
        glyph = &g_aHiResOffscreenGlyphs[index];
        inside = glyph->buffer == source->pixels && glyph->stride == stride &&
            glyph->x >= left && glyph->y >= top &&
            glyph->x + glyph->width - 1 <= right &&
            glyph->y + glyph->height - 1 <= bottom;
        if (!inside)
            continue;
        x = glyph->x + offsetX;
        y = glyph->y + offsetY;
        if (x < 0 || y < 0 || x + glyph->width > SDL_PORT_FRAME_WIDTH ||
            y + glyph->height > SDL_PORT_FRAME_HEIGHT)
            continue;
        copied = *glyph;
        copied.buffer = screen;
        copied.stride = SDL_PORT_FRAME_WIDTH;
        copied.x = (short)x;
        copied.y = (short)y;
        StoreHiResGlyph(g_aHiResGlyphs, &g_nHiResGlyphCount,
                        GL_HIRES_GLYPH_CAPACITY, &copied);
    }
}

/* CopyViewportContents puts the source's corner on the destination's. */
void SdlCopyHiResGlyphs(const Viewport *source, const Viewport *destination)
{
    if (source == 0 || destination == 0 ||
        destination->pixels != stScreenView.pixels)
        return;
    CopyHiResGlyphsToScreen(source, source->left, source->top, source->right,
                            source->bottom, destination->left - source->left,
                            destination->top - source->top);
}

/* Where the flight pointer the HUD drew this frame covers the space buffer
   being copied: the pointer's own pixels, as RestoreSpriteBackground puts a
   save area back. Only for a buffer laid out as the screen is. */
static void NoteHiResFlightPointer(const Viewport *source, int originX,
                                   int originY)
{
    unsigned char coverage[sizeof(abCursorSaveArea)];
    Viewport viewport;

    g_bHiResPointerShown = 0;
    if (!g_bHiResPointerDrawn)
        return;
    g_bHiResPointerDrawn = 0;
    if (!SdlHiResHudFontsActive() || stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0 ||
        stMouseCursorState.viewport->pixels != source->pixels ||
        source->rowOffsets == 0 || source->top < 0 ||
        source->top >= source->bottom ||
        source->bottom >= SDL_PORT_FRAME_HEIGHT ||
        source->rowOffsets[source->top] !=
            (long)source->top * SDL_PORT_FRAME_WIDTH ||
        source->rowOffsets[source->top + 1] !=
            (long)(source->top + 1) * SDL_PORT_FRAME_WIDTH ||
        source->rowOffsets[source->bottom] !=
            (long)source->bottom * SDL_PORT_FRAME_WIDTH)
        return;
    memset(g_abHiResPointerMask, 0, sizeof(g_abHiResPointerMask));
    memset(coverage, 0xff, sizeof(coverage));
    viewport = *source;
    viewport.pixels = g_abHiResPointerMask;
    viewport.allocation = g_abHiResPointerMask;
    RestoreSpriteBackground(&viewport, coverage, (short)nStoredMouseCursorPosX,
                            (short)nSavedMouseCursorY,
                            stMouseCursorState.shape,
                            stMouseCursorState.frame);
    memcpy(g_abHiResPointerColours, source->pixels,
           (size_t)(source->bottom + 1) * SDL_PORT_FRAME_WIDTH);
    g_pHiResPointerBuffer = source->pixels;
    g_nHiResPointerOffsetX = originX;
    g_nHiResPointerOffsetY = originY;
    g_bHiResPointerShown = 1;
}

/* BlitViewportScanlineRuns copies the space buffer's view window, shifted
   by the geometry's origin. Text it leaves under the cockpit no longer
   matches the presented frame and is dropped there. */
void SdlCopyHiResGlyphRuns(const Viewport *source, const Viewport *destination,
                           int originX, int originY)
{
    if (source == 0 || destination == 0 ||
        destination->pixels != stScreenView.pixels)
        return;
    NoteHiResFlightPointer(source, originX, originY);
    CopyHiResGlyphsToScreen(source, 0, 0, 0x7fff, 0x7fff, originX, originY);
}

static int HiResGlyphStillShown(const GlHiResGlyph *glyph,
                                const unsigned char *frame)
{
    return HiResGlyphMatches(glyph, frame, SDL_PORT_FRAME_WIDTH, glyph->x,
                             glyph->y, 2);
}


/* The hi-res font keeps its letter spacing in each glyph's leading columns. */
static int HiResLeadingBlank(const unsigned char *bitmap, int width,
                             int height)
{
    int column;
    int row;
    unsigned char value;

    for (column = 0; column < width; column++) {
        for (row = 0; row < height; row++) {
            value = bitmap[row * width + column];
            if (value != 0xff && value != g_pHiResFont[3])
                return column;
        }
    }
    return width;
}

static void SetHiResPixel(int x, int y, unsigned char index)
{
    unsigned char *pixel;

    if (x < 0 || y < 0 || x >= GL_HIRES_WIDTH || y >= GL_HIRES_HEIGHT)
        return;
    pixel = g_renderer.hiResLayer + ((size_t)y * GL_HIRES_WIDTH + x) * 2;
    pixel[0] = index;
    pixel[1] = 0xff;
}

/* Reuse the cursor's restore spans as an opacity mask, including clipping
   and its hotspot: comparing the frame before and after drawing would miss
   opaque cursor pixels whose colour matches the background. Copy the drawn
   cursor above the text at the base frame's 2x scale. */
static void CompositeHiResMouseCursor(const unsigned char *pixels)
{
    unsigned char coverage[sizeof(abCursorSaveArea)];
    Viewport viewport;
    int x;
    int y;
    int column;
    int row;

    if (bEventMouseCursorDrawnFlag == 0 ||
        stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0 ||
        stMouseCursorState.viewport->pixels != pixels)
        return;
    /* Glyph validation has finished with this scratch frame. */
    memset(g_renderer.hiResCheckFrame, 0, sizeof(g_renderer.hiResCheckFrame));
    memset(coverage, 0xff, sizeof(coverage));
    viewport = *stMouseCursorState.viewport;
    viewport.pixels = g_renderer.hiResCheckFrame;
    viewport.allocation = g_renderer.hiResCheckFrame;
    RestoreSpriteBackground(&viewport, coverage, nMousePointerDrawnX,
                            nMousePointerDrawnY, stMouseCursorState.shape,
                            stMouseCursorState.frame);
    for (y = 0; y < SDL_PORT_FRAME_HEIGHT; y++) {
        for (x = 0; x < SDL_PORT_FRAME_WIDTH; x++) {
            if (g_renderer.hiResCheckFrame[y * SDL_PORT_FRAME_WIDTH + x] == 0)
                continue;
            for (row = 0; row < GL_HIRES_SCALE; row++) {
                for (column = 0; column < GL_HIRES_SCALE; column++)
                    SetHiResPixel(x * GL_HIRES_SCALE + column,
                                  y * GL_HIRES_SCALE + row,
                                  pixels[y * SDL_PORT_FRAME_WIDTH + x]);
            }
        }
    }
}

/* The flight pointer back above the text, where the screen still shows it:
   the window shows the space buffer, the cockpit around it does not. */
static void CompositeHiResFlightPointer(const unsigned char *pixels)
{
    int column;
    int row;
    int x;
    int y;
    int screenX;
    int screenY;

    if (!g_bHiResPointerShown)
        return;
    g_bHiResPointerShown = 0;
    for (y = 0; y < SDL_PORT_FRAME_HEIGHT; y++) {
        for (x = 0; x < SDL_PORT_FRAME_WIDTH; x++) {
            if (g_abHiResPointerMask[y * SDL_PORT_FRAME_WIDTH + x] == 0)
                continue;
            screenX = x + g_nHiResPointerOffsetX;
            screenY = y + g_nHiResPointerOffsetY;
            if (screenX < 0 || screenY < 0 ||
                screenX >= SDL_PORT_FRAME_WIDTH ||
                screenY >= SDL_PORT_FRAME_HEIGHT ||
                pixels[screenY * SDL_PORT_FRAME_WIDTH + screenX] !=
                    g_abHiResPointerColours[y * SDL_PORT_FRAME_WIDTH + x])
                continue;
            for (row = 0; row < GL_HIRES_SCALE; row++) {
                for (column = 0; column < GL_HIRES_SCALE; column++)
                    SetHiResPixel(screenX * GL_HIRES_SCALE + column,
                                  screenY * GL_HIRES_SCALE + row,
                                  pixels[screenY * SDL_PORT_FRAME_WIDTH +
                                         screenX]);
            }
        }
    }
}

/* A letter something is drawn over, as the weapon display's missile crosses
   its text, is hidden but keeps its place in its line: the game restores
   what it covered without drawing the letter again. It is forgotten once
   newer text is drawn in its line, or once the whole line has been gone
   for a while. */
static void DropHiddenHiResGlyphs(void)
{
    const GlHiResGlyph *glyph;
    unsigned int first;
    unsigned int last;
    unsigned int index;
    unsigned int kept;
    unsigned int newest;
    int shown;

    kept = 0;
    for (first = 0; first < g_nHiResGlyphCount; first = last) {
        shown = 0;
        newest = 0;
        for (last = first; last < g_nHiResGlyphCount &&
             (last == first || HiResGlyphsAdjacent(&g_aHiResGlyphs[last - 1],
                                                   &g_aHiResGlyphs[last]));
             last++) {
            if (g_aHiResGlyphs[last].misses == 0) {
                shown = 1;
                if (g_aHiResGlyphs[last].stamp > newest)
                    newest = g_aHiResGlyphs[last].stamp;
            }
        }
        for (index = first; index < last; index++) {
            glyph = &g_aHiResGlyphs[index];
            if (glyph->misses != 0 &&
                (shown ? glyph->stamp < newest
                       : glyph->misses > GL_HIRES_HIDDEN_FRAME_LIMIT))
                continue;
            if (kept != index)
                g_aHiResGlyphs[kept] = *glyph;
            kept++;
        }
    }
    g_nHiResGlyphCount = kept;
}

/* Forgets the glyphs the frame has stopped showing and hides those it does
   not show now. The shown glyphs' letters are erased from the presented
   frame, so recorded sprites still show between transparent HUD letters,
   and the layer gets the hi-res glyphs. A
   run of adjacent cells is set with the hi-res font's own spacing, centred
   when the game centred it and otherwise from its left edge. A run a few
   pixels wider than its cells, as digits are, starts in its first glyph's
   leading blank columns and then closes gaps of two or more blank columns;
   ink is never cut. */
static void BuildHiResLayer(const unsigned char *pixels, unsigned char *frame)
{
    const GlHiResGlyph *glyph;
    const unsigned char *bitmap;
    unsigned int index;
    unsigned int kept;
    unsigned char value;
    unsigned char fontInk;
    unsigned char fontBackground;
    int width;
    int height;
    int left;
    int top;
    int total;
    int overflow;
    int lead;
    int row;
    int column;
    unsigned int first;
    unsigned int last;

    g_renderer.hiResLayerVisible = 0;
    if (!SdlHiResHudFontsActive()) {
        g_nHiResGlyphCount = 0;
        g_nHiResOffscreenGlyphCount = 0;
        g_bHiResPointerShown = 0;
        return;
    }
    g_nHiResPresentCount++;
    CopyBaseWithoutMouse(g_renderer.hiResCheckFrame, pixels);
    for (index = 0; index < g_nHiResGlyphCount; index++) {
        if (HiResGlyphStillShown(&g_aHiResGlyphs[index],
                                 g_renderer.hiResCheckFrame))
            g_aHiResGlyphs[index].misses = 0;
        else if (g_aHiResGlyphs[index].misses < 0xff)
            g_aHiResGlyphs[index].misses++;
    }
    DropHiResSpaceRuns(g_aHiResGlyphs, &g_nHiResGlyphCount);
    DropHiddenHiResGlyphs();
    kept = g_nHiResGlyphCount;
    if (kept == 0 || g_pHiResFont == 0) {
        g_bHiResPointerShown = 0;
        return;
    }
    memset(g_renderer.hiResLayer, 0, sizeof(g_renderer.hiResLayer));
    for (index = 0; index < kept; index++) {
        glyph = &g_aHiResGlyphs[index];
        if (glyph->misses != 0)
            continue;
        for (row = 0; row < glyph->height; row++) {
            for (column = 0; column < glyph->width; column++) {
                if (!HiResGlyphInk(glyph, row * glyph->width + column))
                    continue;
                frame[(glyph->y + row) * SDL_PORT_FRAME_WIDTH + glyph->x +
                      column] =
                    glyph->background != 0xff
                        ? glyph->background
                        : glyph->before[row * glyph->width + column];
            }
        }
    }
    fontInk = g_pHiResFont[2];
    fontBackground = g_pHiResFont[3];
    for (first = 0; first < kept; first = last) {
        last = first + 1;
        while (last < kept && HiResGlyphsAdjacent(&g_aHiResGlyphs[last - 1],
                                                  &g_aHiResGlyphs[last]))
            last++;
        total = 0;
        for (index = first; index < last; index++) {
            if (FindHiResGlyph(g_aHiResGlyphs[index].character, &width,
                               &height) != 0)
                total += width;
        }
        left = g_aHiResGlyphs[first].x * GL_HIRES_SCALE;
        overflow = total -
            ((g_aHiResGlyphs[last - 1].x + g_aHiResGlyphs[last - 1].width) *
                 GL_HIRES_SCALE - left);
        if (g_aHiResGlyphs[first].centered) {
            left -= overflow / 2;
            overflow = 0;
        } else if (overflow > 0) {
            bitmap = FindHiResGlyph(g_aHiResGlyphs[first].character, &width,
                                    &height);
            lead = bitmap != 0 ? HiResLeadingBlank(bitmap, width, height) : 0;
            if (lead > overflow)
                lead = overflow;
            left -= lead;
            overflow -= lead;
        }
        for (index = first; index < last; index++) {
            glyph = &g_aHiResGlyphs[index];
            bitmap = FindHiResGlyph(glyph->character, &width, &height);
            if (bitmap == 0)
                continue;
            if (overflow > 0 && index > first &&
                HiResLeadingBlank(bitmap, width, height) >= 2) {
                left--;
                overflow--;
            }
            top = glyph->y * GL_HIRES_SCALE;
            if (height < glyph->height * GL_HIRES_SCALE)
                top += (glyph->height * GL_HIRES_SCALE - height) / 2;
            for (row = 0; row < height && glyph->misses == 0; row++) {
                for (column = 0; column < width; column++) {
                    value = bitmap[row * width + column];
                    if (value == 0xff || value == fontBackground)
                        continue;
                    SetHiResPixel(left + column, top + row,
                                  value == fontInk ? glyph->colour : value);
                }
            }
            left += width;
        }
    }
    CompositeHiResMouseCursor(pixels);
    CompositeHiResFlightPointer(pixels);
    g_renderer.hiResLayerVisible = 1;
    g_renderer.hiResLayerDirty = 1;
}

static void DrawHiResLayer(void)
{
    const GlVertex vertices[4] = {
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 199.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f},
        {319.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f},
        {319.5f, 199.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f}};

    g_renderer.gl.ActiveTexture(GL_TEXTURE4);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.overlayTexture);
    if (g_renderer.hiResLayerDirty) {
        g_renderer.gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RG8, GL_HIRES_WIDTH,
                                 GL_HIRES_HEIGHT, 0, GL_RG, GL_UNSIGNED_BYTE,
                                 g_renderer.hiResLayer);
        g_renderer.hiResLayerDirty = 0;
    }
    g_renderer.gl.Disable(GL_BLEND);
    g_renderer.gl.UseProgram(g_renderer.overlayProgram);
    g_renderer.gl.ActiveTexture(GL_TEXTURE1);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.paletteTexture);
    g_renderer.gl.Uniform1i(g_renderer.overlayIndicesUniform, 4);
    g_renderer.gl.Uniform1i(g_renderer.overlayPaletteUniform, 1);
    DrawGlQuad(vertices);
}

static void *LoadGlFunction(const char *name, int *functionsAvailable)
{
    void *function;

    if (!*functionsAvailable)
        return 0;
    function = SDL_GL_GetProcAddress(name);
    if (function == 0) {
        fprintf(stderr, "OpenGL function %s is unavailable.\n", name);
        *functionsAvailable = 0;
    }
    return function;
}

static int LoadGlFunctions(void)
{
    int functionsAvailable;

    functionsAvailable = 1;
#define LOAD_GL_FUNCTION(member, type, name) \
    g_renderer.gl.member = (type)LoadGlFunction(name, &functionsAvailable)

    LOAD_GL_FUNCTION(ActiveTexture, GlActiveTextureProc,
                         "glActiveTexture");
    LOAD_GL_FUNCTION(AttachShader, GlAttachShaderProc,
                         "glAttachShader");
    LOAD_GL_FUNCTION(BindAttribLocation, GlBindAttribLocationProc,
                         "glBindAttribLocation");
    LOAD_GL_FUNCTION(BindBuffer, GlBindBufferProc, "glBindBuffer");
    LOAD_GL_FUNCTION(BindTexture, GlBindTextureProc, "glBindTexture");
    LOAD_GL_FUNCTION(BindVertexArray, GlBindVertexArrayProc,
                         "glBindVertexArray");
    LOAD_GL_FUNCTION(BlendFunc, GlBlendFuncProc, "glBlendFunc");
    LOAD_GL_FUNCTION(BufferData, GlBufferDataProc, "glBufferData");
    LOAD_GL_FUNCTION(Clear, GlClearProc, "glClear");
    LOAD_GL_FUNCTION(ClearColor, GlClearColorProc, "glClearColor");
    LOAD_GL_FUNCTION(CompileShader, GlCompileShaderProc,
                         "glCompileShader");
    LOAD_GL_FUNCTION(CreateProgram, GlCreateProgramProc,
                         "glCreateProgram");
    LOAD_GL_FUNCTION(CreateShader, GlCreateShaderProc,
                         "glCreateShader");
    LOAD_GL_FUNCTION(DeleteBuffers, GlDeleteBuffersProc,
                         "glDeleteBuffers");
    LOAD_GL_FUNCTION(DeleteProgram, GlDeleteProgramProc,
                         "glDeleteProgram");
    LOAD_GL_FUNCTION(DeleteShader, GlDeleteShaderProc,
                         "glDeleteShader");
    LOAD_GL_FUNCTION(DeleteTextures, GlDeleteTexturesProc,
                         "glDeleteTextures");
    LOAD_GL_FUNCTION(DeleteVertexArrays, GlDeleteVertexArraysProc,
                         "glDeleteVertexArrays");
    LOAD_GL_FUNCTION(Disable, GlDisableProc, "glDisable");
    LOAD_GL_FUNCTION(DrawArrays, GlDrawArraysProc, "glDrawArrays");
    LOAD_GL_FUNCTION(Enable, GlEnableProc, "glEnable");
    LOAD_GL_FUNCTION(EnableVertexAttribArray,
                         GlEnableVertexAttribArrayProc,
                         "glEnableVertexAttribArray");
    LOAD_GL_FUNCTION(GenBuffers, GlGenBuffersProc, "glGenBuffers");
    LOAD_GL_FUNCTION(GenTextures, GlGenTexturesProc, "glGenTextures");
    LOAD_GL_FUNCTION(GenVertexArrays, GlGenVertexArraysProc,
                         "glGenVertexArrays");
    LOAD_GL_FUNCTION(GetProgramInfoLog, GlGetProgramInfoLogProc,
                         "glGetProgramInfoLog");
    LOAD_GL_FUNCTION(GetProgramiv, GlGetProgramivProc,
                         "glGetProgramiv");
    LOAD_GL_FUNCTION(GetShaderInfoLog, GlGetShaderInfoLogProc,
                         "glGetShaderInfoLog");
    LOAD_GL_FUNCTION(GetShaderiv, GlGetShaderivProc, "glGetShaderiv");
    LOAD_GL_FUNCTION(GetIntegerv, GlGetIntegervProc, "glGetIntegerv");
    LOAD_GL_FUNCTION(GetUniformLocation, GlGetUniformLocationProc,
                         "glGetUniformLocation");
    LOAD_GL_FUNCTION(LinkProgram, GlLinkProgramProc, "glLinkProgram");
    LOAD_GL_FUNCTION(PixelStorei, GlPixelStoreiProc, "glPixelStorei");
    LOAD_GL_FUNCTION(ShaderSource, GlShaderSourceProc,
                         "glShaderSource");
    LOAD_GL_FUNCTION(TexImage2D, GlTexImage2DProc, "glTexImage2D");
    LOAD_GL_FUNCTION(TexImage3D, GlTexImage3DProc, "glTexImage3D");
    LOAD_GL_FUNCTION(TexParameteri, GlTexParameteriProc,
                         "glTexParameteri");
    LOAD_GL_FUNCTION(TexSubImage3D, GlTexSubImage3DProc,
                         "glTexSubImage3D");
    LOAD_GL_FUNCTION(Uniform1i, GlUniform1iProc, "glUniform1i");
    LOAD_GL_FUNCTION(UseProgram, GlUseProgramProc, "glUseProgram");
    LOAD_GL_FUNCTION(VertexAttribPointer, GlVertexAttribPointerProc,
                         "glVertexAttribPointer");
    LOAD_GL_FUNCTION(Viewport, GlViewportProc, "glViewport");
#undef LOAD_GL_FUNCTION
    return functionsAvailable;
}

static void TerminateGlInfoLog(GLchar *log, size_t logSize, GLsizei length)
{
    size_t end;

    if (length <= 0)
        end = 0;
    else if ((size_t)length < logSize)
        end = length;
    else
        end = logSize - 1;
    log[end] = 0;
}

static GLuint CompileGlShader(GLenum type, const GLchar *source)
{
    GLchar log[1024];
    GLsizei length;
    GLint status;
    GLuint shader;

    shader = g_renderer.gl.CreateShader(type);
    if (shader == 0)
        return 0;
    g_renderer.gl.ShaderSource(shader, 1, &source, 0);
    g_renderer.gl.CompileShader(shader);
    g_renderer.gl.GetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        length = 0;
        g_renderer.gl.GetShaderInfoLog(shader, sizeof(log) - 1, &length, log);
        TerminateGlInfoLog(log, sizeof(log), length);
        fprintf(stderr, "Sharp-bilinear shader compilation failed: %s\n", log);
        g_renderer.gl.DeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint LinkGlProgram(const GLchar *fragmentSource)
{
    GLchar log[1024];
    GLsizei length;
    GLint status;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

    vertexShader = CompileGlShader(GL_VERTEX_SHADER, g_vertexShaderSource);
    if (vertexShader == 0)
        return 0;
    fragmentShader = CompileGlShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        g_renderer.gl.DeleteShader(vertexShader);
        return 0;
    }
    program = g_renderer.gl.CreateProgram();
    if (program != 0) {
        g_renderer.gl.AttachShader(program, vertexShader);
        g_renderer.gl.AttachShader(program, fragmentShader);
        g_renderer.gl.BindAttribLocation(program, 0, "a_position");
        g_renderer.gl.BindAttribLocation(program, 1, "a_texCoord");
        g_renderer.gl.BindAttribLocation(program, 2, "a_atlasRect");
        g_renderer.gl.BindAttribLocation(program, 3, "a_clip");
        g_renderer.gl.BindAttribLocation(program, 4, "a_spriteInfo");
        g_renderer.gl.LinkProgram(program);
        g_renderer.gl.GetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            length = 0;
            g_renderer.gl.GetProgramInfoLog(program, sizeof(log) - 1, &length,
                                            log);
            TerminateGlInfoLog(log, sizeof(log), length);
            fprintf(stderr, "Sharp-bilinear shader link failed: %s\n", log);
            g_renderer.gl.DeleteProgram(program);
            program = 0;
        }
    }
    g_renderer.gl.DeleteShader(fragmentShader);
    g_renderer.gl.DeleteShader(vertexShader);
    return program;
}

static int LoadGlUniformLocations(void)
{
    g_renderer.baseIndicesUniform =
        g_renderer.gl.GetUniformLocation(g_renderer.baseProgram, "u_indices");
    g_renderer.basePaletteUniform =
        g_renderer.gl.GetUniformLocation(g_renderer.baseProgram, "u_palette");
    g_renderer.spriteIndicesUniform = g_renderer.gl.GetUniformLocation(
        g_renderer.spriteProgram, "u_indices");
    g_renderer.spritePaletteUniform = g_renderer.gl.GetUniformLocation(
        g_renderer.spriteProgram, "u_palette");
    g_renderer.spriteBaseUniform = g_renderer.gl.GetUniformLocation(
        g_renderer.spriteProgram, "u_baseIndices");
    g_renderer.spriteMaskUniform = g_renderer.gl.GetUniformLocation(
        g_renderer.spriteProgram, "u_viewMask");
    g_renderer.spriteClearUniform = g_renderer.gl.GetUniformLocation(
        g_renderer.spriteProgram, "u_clearIndex");
    if (g_renderer.baseIndicesUniform < 0 ||
        g_renderer.basePaletteUniform < 0 ||
        g_renderer.spriteIndicesUniform < 0 ||
        g_renderer.spritePaletteUniform < 0 ||
        g_renderer.spriteBaseUniform < 0 || g_renderer.spriteMaskUniform < 0 ||
        g_renderer.spriteClearUniform < 0) {
        fprintf(stderr, "OpenGL renderer uniforms are unavailable.\n");
        return 0;
    }
    return 1;
}

static void ConfigureGlTexture(GLenum target, GLuint texture)
{
    g_renderer.gl.BindTexture(target, texture);
    g_renderer.gl.TexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    g_renderer.gl.TexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    g_renderer.gl.TexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g_renderer.gl.TexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

static void UploadIndexTexture(GLuint texture, int width, int height,
                               const unsigned char *pixels)
{
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, texture);
    g_renderer.gl.TexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
                             GL_UNSIGNED_BYTE, pixels);
}

static unsigned int GetCachedFrameHash(const struct Shape *shape, short frame,
                                       int dataOffset)
{
    uintptr_t pointerHash;
    unsigned int hash;

    pointerHash = (uintptr_t)shape;
    hash = (pointerHash ^ (pointerHash >> 16));
    hash ^= (unsigned int)(unsigned short)frame * 0x9e3779b1U;
    hash ^= (unsigned int)dataOffset * 0x85ebca6bU;
    hash ^= hash >> 16;
    return hash & (GL_RENDERER_CACHED_FRAME_CAPACITY - 1);
}

static GlCachedFrame *FindCachedFrameSlot(
    struct Shape *shape, short frame, int dataOffset,
    unsigned short shapeTableEnd, int width, int height, short leftExtent,
    short topExtent, int *cacheHit)
{
    GlCachedFrame *cachedFrame;
    unsigned int slot;
    unsigned int probe;

    slot = GetCachedFrameHash(shape, frame, dataOffset);
    probe = 0;
    while (probe < GL_RENDERER_CACHED_FRAME_CAPACITY) {
        cachedFrame = &g_renderer.cachedFrames[slot];
        if (cachedFrame->shape == 0) {
            *cacheHit = 0;
            return cachedFrame;
        }
        if (cachedFrame->shape == shape && cachedFrame->frame == frame &&
            cachedFrame->dataOffset == dataOffset &&
            cachedFrame->shapeTableEnd == shapeTableEnd) {
            *cacheHit = cachedFrame->width == width &&
                        cachedFrame->height == height &&
                        cachedFrame->leftExtent == leftExtent &&
                        cachedFrame->topExtent == topExtent;
            return cachedFrame;
        }
        slot = (slot + 1) & (GL_RENDERER_CACHED_FRAME_CAPACITY - 1);
        probe++;
    }
    return 0;
}

static int ReserveSpriteAtlasRect(int width, int height, short *atlasLayer,
                                  short *atlasLeft, short *atlasTop)
{
    GlAtlasLayer *layer;
    int packedWidth;
    int packedHeight;
    int x;
    int y;
    int layerIndex;

    packedWidth = width + GL_RENDERER_SPRITE_ATLAS_PADDING * 2;
    packedHeight = height + GL_RENDERER_SPRITE_ATLAS_PADDING * 2;
    if (packedWidth > g_renderer.spriteAtlasSize ||
        packedHeight > g_renderer.spriteAtlasSize)
        return 0;
    layerIndex = 0;
    while (layerIndex < g_renderer.spriteAtlasLayerCount) {
        layer = &g_renderer.atlasLayers[layerIndex];
        x = layer->nextX;
        y = layer->nextY;
        if (x + packedWidth > g_renderer.spriteAtlasSize) {
            x = 0;
            y += layer->rowHeight;
        }
        if (y + packedHeight <= g_renderer.spriteAtlasSize) {
            *atlasLayer = layerIndex;
            *atlasLeft = (x + GL_RENDERER_SPRITE_ATLAS_PADDING);
            *atlasTop = (y + GL_RENDERER_SPRITE_ATLAS_PADDING);
            layer->nextX = x + packedWidth;
            layer->nextY = y;
            if (x == 0 || packedHeight > layer->rowHeight)
                layer->rowHeight = packedHeight;
            return 1;
        }
        layerIndex++;
    }
    return 0;
}

static const GlCachedFrame *CacheShapeFrame(
    struct Shape *shape, short frame, int dataOffset,
    unsigned short shapeTableEnd, int width, int height, short leftExtent,
    short topExtent, size_t pixelCount)
{
    GlCachedFrame *cachedFrame;
    unsigned char *resizedScratch;
    short atlasLayer;
    short atlasLeft;
    short atlasTop;
    int packedWidth;
    int packedHeight;
    int cacheHit;

    cacheHit = 0;
    cachedFrame = FindCachedFrameSlot(
        shape, frame, dataOffset, shapeTableEnd, width, height, leftExtent,
        topExtent, &cacheHit);
    if (cachedFrame == 0) {
        g_renderer.atlasResetPending = 1;
        return 0;
    }
    if (cacheHit)
        return cachedFrame;

    packedWidth = width + GL_RENDERER_SPRITE_ATLAS_PADDING * 2;
    packedHeight = height + GL_RENDERER_SPRITE_ATLAS_PADDING * 2;
    if (packedWidth > g_renderer.spriteAtlasSize ||
        packedHeight > g_renderer.spriteAtlasSize)
        return 0;
    if (g_renderer.decodeScratchCapacity < pixelCount) {
        resizedScratch =
            realloc(g_renderer.decodeScratch, pixelCount);
        if (resizedScratch == 0)
            return 0;
        g_renderer.decodeScratch = resizedScratch;
        g_renderer.decodeScratchCapacity = pixelCount;
    }
    if (!ReserveSpriteAtlasRect(width, height, &atlasLayer, &atlasLeft,
                                &atlasTop)) {
        g_renderer.atlasResetPending = 1;
        return 0;
    }

    memset(g_renderer.decodeScratch, 0xff, pixelCount);
    DecodeShapeFrame(shape, frame, g_renderer.decodeScratch, width,
                     height, leftExtent, topExtent);
    g_renderer.gl.ActiveTexture(GL_TEXTURE0);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D_ARRAY,
                              g_renderer.spriteAtlasTexture);
    g_renderer.gl.TexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0, atlasLeft, atlasTop, atlasLayer, width, height,
        1, GL_RED, GL_UNSIGNED_BYTE, g_renderer.decodeScratch);

    cachedFrame->shape = shape;
    cachedFrame->dataOffset = dataOffset;
    cachedFrame->width = width;
    cachedFrame->height = height;
    cachedFrame->shapeTableEnd = shapeTableEnd;
    cachedFrame->leftExtent = leftExtent;
    cachedFrame->topExtent = topExtent;
    cachedFrame->frame = frame;
    cachedFrame->atlasLayer = atlasLayer;
    cachedFrame->atlasLeft = atlasLeft;
    cachedFrame->atlasTop = atlasTop;
    return cachedFrame;
}

static void UploadPaletteTexture(const unsigned char *palette)
{
    unsigned char rgba[256 * 4];
    int index;

    index = 0;
    while (index < 256) {
        rgba[index * 4] = palette[index * 4 + 2];
        rgba[index * 4 + 1] = palette[index * 4 + 1];
        rgba[index * 4 + 2] = palette[index * 4];
        rgba[index * 4 + 3] = 0xff;
        index++;
    }
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.paletteTexture);
    g_renderer.gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 1, 0, GL_RGBA,
                             GL_UNSIGNED_BYTE, rgba);
}

static void DrawGlQuad(const GlVertex *vertices)
{
    g_renderer.gl.BindBuffer(GL_ARRAY_BUFFER, g_renderer.vertexBuffer);
    g_renderer.gl.BufferData(GL_ARRAY_BUFFER, sizeof(GlVertex) * 4,
                             vertices, GL_STREAM_DRAW);
    g_renderer.gl.DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void DrawBaseFrame(void)
{
    const GlVertex vertices[4] = {
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 199.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f},
        {319.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f},
        {319.5f, 199.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f}};

    g_renderer.gl.Disable(GL_BLEND);
    g_renderer.gl.UseProgram(g_renderer.baseProgram);
    g_renderer.gl.ActiveTexture(GL_TEXTURE0);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.baseTexture);
    g_renderer.gl.ActiveTexture(GL_TEXTURE1);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.paletteTexture);
    g_renderer.gl.Uniform1i(g_renderer.baseIndicesUniform, 0);
    g_renderer.gl.Uniform1i(g_renderer.basePaletteUniform, 1);
    DrawGlQuad(vertices);
}

static void TransformSpriteVertex(GlVertex *vertex, float sourceX,
                                  float sourceY, float textureX,
                                  float textureY, float originX, float originY,
                                  float cosine, float sine, float scaleX,
                                  float scaleY)
{
    sourceX *= scaleX;
    sourceY *= scaleY;
    vertex->x = originX + sourceX * cosine - sourceY * sine;
    vertex->y = originY + sourceX * sine + sourceY * cosine;
    vertex->u = textureX;
    vertex->v = textureY;
}

static void SetSpriteVertexMetadata(GlVertex *vertex,
                                    const GlSprite *sprite,
                                    float outputScale)
{
    const GlCachedFrame *cachedFrame;

    cachedFrame = sprite->cachedFrame;
    vertex->atlasLeft = cachedFrame->atlasLeft;
    vertex->atlasTop = cachedFrame->atlasTop;
    vertex->atlasWidth = cachedFrame->width;
    vertex->atlasHeight = cachedFrame->height;
    vertex->clipLeft = (float)sprite->clipLeft - 0.5f;
    vertex->clipTop = (float)sprite->clipTop - 0.5f;
    vertex->clipRight = (float)sprite->clipRight + 0.5f;
    vertex->clipBottom = (float)sprite->clipBottom + 0.5f;
    vertex->magnification =
        outputScale *
        (sprite->scale < 0 ? -(float)sprite->scale : (float)sprite->scale) /
        256.0f;
    vertex->atlasLayer = cachedFrame->atlasLayer;
}

static void BuildRecordedSpriteVertices(const GlSprite *sprite,
                                        float outputScale,
                                        GlVertex *vertices)
{
    const GlCachedFrame *cachedFrame;
    GlVertex corners[4];
    float left;
    float top;
    float right;
    float bottom;
    float scaleX;
    float scaleY;
    float cosine;
    float sine;
    int fixedCosine;
    int fixedSine;
    int corner;

    cachedFrame = sprite->cachedFrame;
    GetRLETransformTrig(sprite->angle * 10, &fixedCosine, &fixedSine);
    cosine = (float)fixedCosine / 65536.0f;
    sine = (float)fixedSine / 65536.0f;
    scaleX = (float)sprite->scale / 256.0f;
    scaleY = scaleX;
    if ((sprite->flip & 0x10) != 0)
        scaleX = -scaleX;
    if ((sprite->flip & 0x20) != 0)
        scaleY = -scaleY;
    left = -(float)cachedFrame->leftExtent - 0.5f;
    top = -(float)cachedFrame->topExtent - 0.5f;
    right = left + (float)cachedFrame->width;
    bottom = top + (float)cachedFrame->height;
    TransformSpriteVertex(&corners[0], left, top, 0.0f, 0.0f,
                          sprite->x, sprite->y, cosine, sine,
                          scaleX, scaleY);
    TransformSpriteVertex(&corners[1], left, bottom, 0.0f, 1.0f,
                          sprite->x, sprite->y, cosine, sine,
                          scaleX, scaleY);
    TransformSpriteVertex(&corners[2], right, top, 1.0f, 0.0f,
                          sprite->x, sprite->y, cosine, sine,
                          scaleX, scaleY);
    TransformSpriteVertex(&corners[3], right, bottom, 1.0f, 1.0f,
                          sprite->x, sprite->y, cosine, sine,
                          scaleX, scaleY);
    corner = 0;
    while (corner < 4) {
        SetSpriteVertexMetadata(&corners[corner], sprite, outputScale);
        corner++;
    }
    vertices[0] = corners[0];
    vertices[1] = corners[1];
    vertices[2] = corners[2];
    vertices[3] = corners[2];
    vertices[4] = corners[1];
    vertices[5] = corners[3];
}

static void DrawRecordedSprites(float outputScale)
{
    unsigned int index;

    if (g_renderer.spriteCount == 0)
        return;
    g_renderer.gl.Enable(GL_BLEND);
    g_renderer.gl.BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    g_renderer.gl.UseProgram(g_renderer.spriteProgram);
    g_renderer.gl.ActiveTexture(GL_TEXTURE0);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D_ARRAY,
                              g_renderer.spriteAtlasTexture);
    g_renderer.gl.ActiveTexture(GL_TEXTURE1);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.paletteTexture);
    g_renderer.gl.ActiveTexture(GL_TEXTURE2);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.baseTexture);
    g_renderer.gl.ActiveTexture(GL_TEXTURE3);
    g_renderer.gl.BindTexture(GL_TEXTURE_2D, g_renderer.maskTexture);
    g_renderer.gl.Uniform1i(g_renderer.spriteIndicesUniform, 0);
    g_renderer.gl.Uniform1i(g_renderer.spritePaletteUniform, 1);
    g_renderer.gl.Uniform1i(g_renderer.spriteBaseUniform, 2);
    g_renderer.gl.Uniform1i(g_renderer.spriteMaskUniform, 3);
    g_renderer.gl.Uniform1i(g_renderer.spriteClearUniform,
                            g_renderer.spaceClearColour);
    index = 0;
    while (index < g_renderer.spriteCount) {
        BuildRecordedSpriteVertices(
            &g_renderer.recordedSprites[index], outputScale,
            &g_renderer.spriteVertices[(size_t)index * 6U]);
        index++;
    }
    g_renderer.gl.BindBuffer(GL_ARRAY_BUFFER, g_renderer.vertexBuffer);
    g_renderer.gl.BufferData(
        GL_ARRAY_BUFFER,
        (GLsizeiptr)(sizeof(GlVertex) *
                     (size_t)g_renderer.spriteCount * 6U),
        g_renderer.spriteVertices, GL_STREAM_DRAW);
    g_renderer.gl.DrawArrays(GL_TRIANGLES, 0,
                             (GLsizei)(g_renderer.spriteCount * 6));
    g_renderer.gl.Disable(GL_BLEND);
}

static int RenderGlFrame(const unsigned char *pixels,
                         const unsigned char *palette, int drawSprites)
{
    int drawableWidth;
    int drawableHeight;
    int viewportLeft;
    int viewportBottom;
    int viewportWidth;
    int viewportHeight;

    if (g_renderer.window == 0 || g_renderer.context == 0)
        return 0;
    if (SDL_GL_MakeCurrent(g_renderer.window, g_renderer.context) != 0)
        return 0;
    SDL_GL_GetDrawableSize(g_renderer.window, &drawableWidth, &drawableHeight);
    if (drawableWidth < 1 || drawableHeight < 1)
        return 0;
    SdlCalculateOutputViewport(drawableWidth, drawableHeight, &viewportLeft,
                                  &viewportBottom, &viewportWidth,
                                  &viewportHeight);
    g_renderer.gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    g_renderer.gl.ActiveTexture(GL_TEXTURE0);
    UploadIndexTexture(g_renderer.baseTexture, SDL_PORT_FRAME_WIDTH,
                       SDL_PORT_FRAME_HEIGHT, pixels);
    g_renderer.gl.ActiveTexture(GL_TEXTURE1);
    UploadPaletteTexture(palette);
    if (drawSprites) {
        g_renderer.gl.ActiveTexture(GL_TEXTURE3);
        UploadIndexTexture(g_renderer.maskTexture, SDL_PORT_FRAME_WIDTH,
                           SDL_PORT_FRAME_HEIGHT, g_renderer.viewMask);
    }
    g_renderer.gl.Viewport(0, 0, drawableWidth, drawableHeight);
    g_renderer.gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    g_renderer.gl.Clear(GL_COLOR_BUFFER_BIT);
    g_renderer.gl.Viewport(viewportLeft, viewportBottom, viewportWidth,
                           viewportHeight);
    g_renderer.gl.BindVertexArray(g_renderer.vertexArray);
    DrawBaseFrame();
    if (drawSprites)
        DrawRecordedSprites((float)viewportWidth / SDL_PORT_FRAME_WIDTH);
    if (g_renderer.hiResLayerVisible)
        DrawHiResLayer();
    SDL_GL_SwapWindow(g_renderer.window);
    return 1;
}

static void BuildSpaceViewMask(const ScreenViewportGeometry *geometry,
                               int viewportMode, int fullViewportCopy)
{
    const short *run;
    int runCount;

    memset(g_renderer.viewMask, 0, sizeof(g_renderer.viewMask));
    if (fullViewportCopy || viewportMode == 5 || geometry == 0) {
        memset(g_renderer.viewMask, 0xff, sizeof(g_renderer.viewMask));
        return;
    }
    /* Packet-backed geometries extend the four-word built-in tail. */
    run = (const short *)((const unsigned char *)geometry +
                          offsetof(ScreenViewportGeometry, fadeData));
    runCount = 0;
    while (*run != -1 && runCount < 2048) {
        int x;
        int y;
        int length;
        int start;
        int available;

        x = *run++;
        y = *run++;
        length = (unsigned short)*run++;
        /* BlitViewportScanlineRuns copies each run with one memcpy into a
           screen whose rows are 320 contiguous bytes, so the count is a byte
           length and a run may cover whole rows: the mode 4 geometry copies
           320x128 as the single run (0, 24, 40960). */
        start = y * SDL_PORT_FRAME_WIDTH + x;
        if (start < 0) {
            length += start;
            start = 0;
        }
        if (start < (int)sizeof(g_renderer.viewMask)) {
            available = (int)sizeof(g_renderer.viewMask) - start;
            if (length > available)
                length = available;
            if (length > 0) {
                memset(g_renderer.viewMask + start, 0xff, (size_t)length);
            }
        }
        runCount++;
    }
}

int SdlGlRendererInitialize(SDL_Window *window)
{
    GLuint textures[4] = {0, 0, 0, 0};
    GLint maxArrayTextureLayers;
    GLint maxTextureSize;

    if (window == 0)
        return 0;
    g_renderer.window = window;
    g_renderer.context = SDL_GL_CreateContext(window);
    if (g_renderer.context == 0) {
        fprintf(stderr, "OpenGL context creation failed: %s\n",
                SDL_GetError());
        SdlGlRendererShutdown();
        return 0;
    }
    if (SDL_GL_MakeCurrent(window, g_renderer.context) != 0 ||
        !LoadGlFunctions()) {
        SdlGlRendererShutdown();
        return 0;
    }
    maxTextureSize = 0;
    maxArrayTextureLayers = 0;
    g_renderer.gl.GetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    g_renderer.gl.GetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,
                              &maxArrayTextureLayers);
    if (maxTextureSize < 1 || maxArrayTextureLayers < 1) {
        fprintf(stderr, "OpenGL texture arrays are unavailable.\n");
        SdlGlRendererShutdown();
        return 0;
    }
    g_renderer.spriteAtlasSize =
        maxTextureSize < GL_RENDERER_SPRITE_ATLAS_MAX_SIZE
            ? maxTextureSize
            : GL_RENDERER_SPRITE_ATLAS_MAX_SIZE;
    g_renderer.spriteAtlasLayerCount =
        maxArrayTextureLayers < GL_RENDERER_SPRITE_ATLAS_LAYER_CAPACITY
            ? maxArrayTextureLayers
            : GL_RENDERER_SPRITE_ATLAS_LAYER_CAPACITY;
    g_renderer.baseProgram = LinkGlProgram(g_baseFragmentShaderSource);
    g_renderer.spriteProgram = LinkGlProgram(g_spriteFragmentShaderSource);
    if (g_renderer.baseProgram == 0 || g_renderer.spriteProgram == 0) {
        SdlGlRendererShutdown();
        return 0;
    }

    g_renderer.gl.GenVertexArrays(1, &g_renderer.vertexArray);
    g_renderer.gl.GenBuffers(1, &g_renderer.vertexBuffer);
    if (g_renderer.vertexArray == 0 || g_renderer.vertexBuffer == 0) {
        fprintf(stderr, "OpenGL renderer buffer creation failed.\n");
        SdlGlRendererShutdown();
        return 0;
    }
    g_renderer.gl.BindVertexArray(g_renderer.vertexArray);
    g_renderer.gl.BindBuffer(GL_ARRAY_BUFFER, g_renderer.vertexBuffer);
    g_renderer.gl.EnableVertexAttribArray(0);
    g_renderer.gl.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                      sizeof(GlVertex), 0);
    g_renderer.gl.EnableVertexAttribArray(1);
    g_renderer.gl.VertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(GlVertex),
        (const void *)offsetof(GlVertex, u));
    g_renderer.gl.EnableVertexAttribArray(2);
    g_renderer.gl.VertexAttribPointer(
        2, 4, GL_FLOAT, GL_FALSE, sizeof(GlVertex),
        (const void *)offsetof(GlVertex, atlasLeft));
    g_renderer.gl.EnableVertexAttribArray(3);
    g_renderer.gl.VertexAttribPointer(
        3, 4, GL_FLOAT, GL_FALSE, sizeof(GlVertex),
        (const void *)offsetof(GlVertex, clipLeft));
    g_renderer.gl.EnableVertexAttribArray(4);
    g_renderer.gl.VertexAttribPointer(
        4, 2, GL_FLOAT, GL_FALSE, sizeof(GlVertex),
        (const void *)offsetof(GlVertex, magnification));

    g_renderer.gl.GenTextures(4, textures);
    g_renderer.baseTexture = textures[0];
    g_renderer.paletteTexture = textures[1];
    g_renderer.maskTexture = textures[2];
    g_renderer.spriteAtlasTexture = textures[3];
    if (g_renderer.baseTexture == 0 || g_renderer.paletteTexture == 0 ||
        g_renderer.maskTexture == 0 ||
        g_renderer.spriteAtlasTexture == 0) {
        fprintf(stderr, "OpenGL renderer texture creation failed.\n");
        SdlGlRendererShutdown();
        return 0;
    }
    ConfigureGlTexture(GL_TEXTURE_2D, g_renderer.baseTexture);
    ConfigureGlTexture(GL_TEXTURE_2D, g_renderer.paletteTexture);
    ConfigureGlTexture(GL_TEXTURE_2D, g_renderer.maskTexture);
    ConfigureGlTexture(GL_TEXTURE_2D_ARRAY,
                       g_renderer.spriteAtlasTexture);
    g_renderer.gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    g_renderer.gl.TexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_R8, g_renderer.spriteAtlasSize,
        g_renderer.spriteAtlasSize, g_renderer.spriteAtlasLayerCount, 0,
        GL_RED, GL_UNSIGNED_BYTE, 0);

    if (!LoadGlUniformLocations()) {
        SdlGlRendererShutdown();
        return 0;
    }
    /* The hi-res HUD text layer is optional: without it the HUD keeps the
       game's own fonts. */
    g_renderer.overlayProgram = LinkGlProgram(g_overlayFragmentShaderSource);
    if (g_renderer.overlayProgram != 0) {
        g_renderer.gl.GenTextures(1, &g_renderer.overlayTexture);
        g_renderer.overlayIndicesUniform = g_renderer.gl.GetUniformLocation(
            g_renderer.overlayProgram, "u_indices");
        g_renderer.overlayPaletteUniform = g_renderer.gl.GetUniformLocation(
            g_renderer.overlayProgram, "u_palette");
        if (g_renderer.overlayTexture == 0 ||
            g_renderer.overlayIndicesUniform < 0 ||
            g_renderer.overlayPaletteUniform < 0) {
            g_renderer.gl.DeleteProgram(g_renderer.overlayProgram);
            g_renderer.overlayProgram = 0;
        } else {
            ConfigureGlTexture(GL_TEXTURE_2D, g_renderer.overlayTexture);
        }
    }
    g_renderer.gl.Disable(GL_DITHER);
    SDL_GL_SetSwapInterval(1);
    memset(g_renderer.viewMask, 0xff, sizeof(g_renderer.viewMask));
    return 1;
}

void SdlGlRendererShutdown(void)
{
    GLuint textures[4];
    int canDeleteResources;

    ReleaseRendererStorage();
    canDeleteResources =
        g_renderer.context != 0 && g_renderer.window != 0 &&
        SDL_GL_MakeCurrent(g_renderer.window, g_renderer.context) == 0;
    if (canDeleteResources && g_renderer.gl.DeleteTextures != 0) {
        textures[0] = g_renderer.baseTexture;
        textures[1] = g_renderer.paletteTexture;
        textures[2] = g_renderer.maskTexture;
        textures[3] = g_renderer.spriteAtlasTexture;
        g_renderer.gl.DeleteTextures(4, textures);
        if (g_renderer.overlayTexture != 0)
            g_renderer.gl.DeleteTextures(1, &g_renderer.overlayTexture);
    }
    if (canDeleteResources && g_renderer.gl.DeleteBuffers != 0 &&
        g_renderer.vertexBuffer != 0) {
        g_renderer.gl.DeleteBuffers(1, &g_renderer.vertexBuffer);
    }
    if (canDeleteResources && g_renderer.gl.DeleteVertexArrays != 0 &&
        g_renderer.vertexArray != 0) {
        g_renderer.gl.DeleteVertexArrays(1, &g_renderer.vertexArray);
    }
    if (canDeleteResources && g_renderer.gl.DeleteProgram != 0) {
        if (g_renderer.baseProgram != 0)
            g_renderer.gl.DeleteProgram(g_renderer.baseProgram);
        if (g_renderer.spriteProgram != 0)
            g_renderer.gl.DeleteProgram(g_renderer.spriteProgram);
        if (g_renderer.overlayProgram != 0)
            g_renderer.gl.DeleteProgram(g_renderer.overlayProgram);
    }
    SdlResetHiResHudFont();
    if (g_renderer.context != 0)
        SDL_GL_DeleteContext(g_renderer.context);
    memset(&g_renderer, 0, sizeof(g_renderer));
}

void SdlGlRendererBeginSpaceFrame(const ScreenViewportGeometry *geometry,
                                     int viewportMode, int fullViewportCopy,
                                     unsigned char spaceClearColour)
{
    ResetRecordedSprites();
    if (g_renderer.atlasResetPending)
        ResetSpriteAtlasCache();
    g_renderer.useSoftwareForRestOfFrame = 0;
    g_renderer.hasLayerSnapshot = 0;
    g_renderer.frameState =
        g_renderer.context != 0 ? GL_RENDERER_FRAME_RECORDING : GL_RENDERER_FRAME_IDLE;
    g_renderer.spaceClearColour = spaceClearColour;
    g_renderer.spaceLayerOffsetX = 0;
    g_renderer.spaceLayerOffsetY = 0;
    if (g_renderer.frameState == GL_RENDERER_FRAME_RECORDING) {
        if (!fullViewportCopy && viewportMode != 5 && geometry != 0) {
            g_renderer.spaceLayerOffsetX = geometry->originX;
            g_renderer.spaceLayerOffsetY = geometry->originY;
        }
        BuildSpaceViewMask(geometry, viewportMode, fullViewportCopy);
    }
}

void SdlGlRendererCompleteSpaceFrame(void)
{
    if (g_renderer.frameState == GL_RENDERER_FRAME_RECORDING)
        g_renderer.frameState = GL_RENDERER_FRAME_COMPLETE;
}

void SdlGlRendererCancelSpaceFrame(void)
{
    ResetRecordedSprites();
    ResetSpriteAtlasCache();
    g_renderer.frameState = GL_RENDERER_FRAME_IDLE;
    g_renderer.hasLayerSnapshot = 0;
    g_renderer.spaceLayerOffsetX = 0;
    g_renderer.spaceLayerOffsetY = 0;
    g_renderer.useSoftwareForRestOfFrame = 0;
}

int SdlGlRendererRecordSpaceSprite(const Viewport *viewport, float x,
                                      float y, struct Shape *shape,
                                      short frame, short angle, short scale,
                                      short flip)
{
    const GlCachedFrame *cachedFrame;
    GlSprite *sprite;
    size_t pixelCount;
    int dataOffset;
    int frameTableOffset;
    unsigned short shapeTableEnd;
    short width;
    short height;
    short leftExtent;
    short topExtent;

    if (g_renderer.frameState != GL_RENDERER_FRAME_RECORDING ||
        g_renderer.useSoftwareForRestOfFrame)
        return 0;
    if (shape == 0 || frame < 0 || scale == 0 || viewport == 0 ||
        viewport->pixels == 0 || viewport->rowOffsets == 0)
        return 0;
    if (flip != 0 && flip != 0x10 && flip != 0x20 && flip != 0x30)
        return 0;
    if (frame >= GetShapeFrameCount(shape))
        return 0;
    if (g_renderer.spriteCount >= GL_RENDERER_RECORDED_SPRITE_CAPACITY)
        return 0;
    width = 0;
    height = 0;
    leftExtent = 0;
    topExtent = 0;
    GetShapeFrameExtents(shape, frame, &width, &height, &leftExtent,
                         &topExtent);
    if (width <= 0 || height <= 0)
        return 0;
    pixelCount = (size_t)(unsigned short)width * (unsigned short)height;
    /* RotateRLEImage rasterises through a 0xfa00-byte scratch buffer and
       refuses any frame larger than that, so the shipped renderer draws
       nothing.  Its unrotated fast path has no such limit.  MIDGAME.V03
       holds two 325x325 frames that reach this, so keep the same silence
       instead of drawing what the original never shows. */
    if (pixelCount > 0xfa00 && !(angle == 0 && scale == 0x100 && flip == 0))
        return 0;
    frameTableOffset = frame * 4 + 4;
    shapeTableEnd = shape->frameOffsets[0];
    memcpy(&dataOffset, (unsigned char *)shape + frameTableOffset, sizeof(dataOffset));
    cachedFrame = CacheShapeFrame(
        shape, frame, dataOffset, shapeTableEnd, width, height, leftExtent,
        topExtent, pixelCount);
    if (cachedFrame == 0) {
        /* Once one object falls back into the indexed base, later objects must
           follow it there so painter ordering remains exact. */
        g_renderer.useSoftwareForRestOfFrame = 1;
        return 0;
    }
    sprite = &g_renderer.recordedSprites[g_renderer.spriteCount];
    sprite->cachedFrame = cachedFrame;
    sprite->x = x + (float)g_renderer.spaceLayerOffsetX;
    sprite->y = y + (float)g_renderer.spaceLayerOffsetY;
    sprite->angle = angle;
    sprite->scale = scale;
    sprite->flip = flip;
    sprite->clipLeft = (viewport->left + g_renderer.spaceLayerOffsetX);
    sprite->clipTop = (viewport->top + g_renderer.spaceLayerOffsetY);
    sprite->clipRight =
        (viewport->right + g_renderer.spaceLayerOffsetX);
    sprite->clipBottom =
        (viewport->bottom + g_renderer.spaceLayerOffsetY);
    g_renderer.spriteCount++;
    return 1;
}

int SdlGlRendererPresent(const unsigned char *pixels,
                            const unsigned char *palette)
{
    int drawSprites;

    if (pixels == 0 || palette == 0 || g_renderer.context == 0)
        return 0;
    /* Mouse redraws can present the same game frame more than once.  Compare
       cursor-free copies so the recorded object layer survives those redraws
       but is discarded as soon as the underlying game frame changes. */
    drawSprites = 0;
    if (g_renderer.frameState == GL_RENDERER_FRAME_COMPLETE) {
        CopyBaseWithoutMouse(g_renderer.spaceFrameBase, pixels);
        g_renderer.hasLayerSnapshot = 1;
        g_renderer.frameState = GL_RENDERER_FRAME_IDLE;
        drawSprites = 1;
    } else if (g_renderer.frameState == GL_RENDERER_FRAME_IDLE &&
               g_renderer.hasLayerSnapshot) {
        CopyBaseWithoutMouse(g_renderer.presentedFrame, pixels);
        if (memcmp(g_renderer.spaceFrameBase, g_renderer.presentedFrame,
                   sizeof(g_renderer.spaceFrameBase)) == 0) {
            drawSprites = 1;
        } else {
            ResetRecordedSprites();
            g_renderer.hasLayerSnapshot = 0;
        }
    }
    memcpy(g_renderer.presentedFrame, pixels,
           sizeof(g_renderer.presentedFrame));
    BuildHiResLayer(pixels, g_renderer.presentedFrame);
    memcpy(g_renderer.presentedPalette, palette,
           sizeof(g_renderer.presentedPalette));
    g_renderer.hasLastFrame = 1;
    return RenderGlFrame(g_renderer.presentedFrame,
                         g_renderer.presentedPalette, drawSprites);
}

void SdlGlRendererWaitForVerticalBlank(void)
{
    if (!g_renderer.hasLastFrame ||
        !RenderGlFrame(g_renderer.presentedFrame, g_renderer.presentedPalette,
                       g_renderer.hasLayerSnapshot))
        SDL_Delay(1);
}
