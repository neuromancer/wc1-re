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

typedef void (APIENTRY *Wc1GlActiveTextureProc)(GLenum texture);
typedef void (APIENTRY *Wc1GlAttachShaderProc)(GLuint program, GLuint shader);
typedef void (APIENTRY *Wc1GlBindAttribLocationProc)(GLuint program,
                                                    GLuint index,
                                                    const GLchar *name);
typedef void (APIENTRY *Wc1GlBindBufferProc)(GLenum target, GLuint buffer);
typedef void (APIENTRY *Wc1GlBindTextureProc)(GLenum target, GLuint texture);
typedef void (APIENTRY *Wc1GlBindVertexArrayProc)(GLuint array);
typedef void (APIENTRY *Wc1GlBlendFuncProc)(GLenum source, GLenum destination);
typedef void (APIENTRY *Wc1GlBufferDataProc)(GLenum target, GLsizeiptr size,
                                            const void *data, GLenum usage);
typedef void (APIENTRY *Wc1GlClearProc)(GLbitfield mask);
typedef void (APIENTRY *Wc1GlClearColorProc)(GLfloat red, GLfloat green,
                                            GLfloat blue, GLfloat alpha);
typedef void (APIENTRY *Wc1GlCompileShaderProc)(GLuint shader);
typedef GLuint (APIENTRY *Wc1GlCreateProgramProc)(void);
typedef GLuint (APIENTRY *Wc1GlCreateShaderProc)(GLenum type);
typedef void (APIENTRY *Wc1GlDeleteBuffersProc)(GLsizei count,
                                               const GLuint *buffers);
typedef void (APIENTRY *Wc1GlDeleteProgramProc)(GLuint program);
typedef void (APIENTRY *Wc1GlDeleteShaderProc)(GLuint shader);
typedef void (APIENTRY *Wc1GlDeleteTexturesProc)(GLsizei count,
                                                const GLuint *textures);
typedef void (APIENTRY *Wc1GlDeleteVertexArraysProc)(GLsizei count,
                                                    const GLuint *arrays);
typedef void (APIENTRY *Wc1GlDisableProc)(GLenum capability);
typedef void (APIENTRY *Wc1GlDrawArraysProc)(GLenum mode, GLint first,
                                            GLsizei count);
typedef void (APIENTRY *Wc1GlEnableProc)(GLenum capability);
typedef void (APIENTRY *Wc1GlEnableVertexAttribArrayProc)(GLuint index);
typedef void (APIENTRY *Wc1GlGenBuffersProc)(GLsizei count, GLuint *buffers);
typedef void (APIENTRY *Wc1GlGenTexturesProc)(GLsizei count, GLuint *textures);
typedef void (APIENTRY *Wc1GlGenVertexArraysProc)(GLsizei count,
                                                 GLuint *arrays);
typedef void (APIENTRY *Wc1GlGetProgramInfoLogProc)(GLuint program,
                                                   GLsizei bufferSize,
                                                   GLsizei *length,
                                                   GLchar *log);
typedef void (APIENTRY *Wc1GlGetProgramivProc)(GLuint program, GLenum name,
                                              GLint *value);
typedef void (APIENTRY *Wc1GlGetShaderInfoLogProc)(GLuint shader,
                                                  GLsizei bufferSize,
                                                  GLsizei *length,
                                                  GLchar *log);
typedef void (APIENTRY *Wc1GlGetShaderivProc)(GLuint shader, GLenum name,
                                             GLint *value);
typedef void (APIENTRY *Wc1GlGetIntegervProc)(GLenum name, GLint *value);
typedef GLint (APIENTRY *Wc1GlGetUniformLocationProc)(GLuint program,
                                                     const GLchar *name);
typedef void (APIENTRY *Wc1GlLinkProgramProc)(GLuint program);
typedef void (APIENTRY *Wc1GlPixelStoreiProc)(GLenum name, GLint value);
typedef void (APIENTRY *Wc1GlShaderSourceProc)(GLuint shader, GLsizei count,
                                              const GLchar *const *source,
                                              const GLint *length);
typedef void (APIENTRY *Wc1GlTexImage2DProc)(GLenum target, GLint level,
                                            GLint internalFormat,
                                            GLsizei width, GLsizei height,
                                            GLint border, GLenum format,
                                            GLenum type, const void *pixels);
typedef void (APIENTRY *Wc1GlTexImage3DProc)(GLenum target, GLint level,
                                            GLint internalFormat,
                                            GLsizei width, GLsizei height,
                                            GLsizei depth, GLint border,
                                            GLenum format, GLenum type,
                                            const void *pixels);
typedef void (APIENTRY *Wc1GlTexParameteriProc)(GLenum target, GLenum name,
                                               GLint value);
typedef void (APIENTRY *Wc1GlTexSubImage3DProc)(
    GLenum target, GLint level, GLint xOffset, GLint yOffset, GLint zOffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *pixels);
typedef void (APIENTRY *Wc1GlUniform1iProc)(GLint location, GLint value);
typedef void (APIENTRY *Wc1GlUseProgramProc)(GLuint program);
typedef void (APIENTRY *Wc1GlVertexAttribPointerProc)(GLuint index, GLint size,
                                                     GLenum type,
                                                     GLboolean normalized,
                                                     GLsizei stride,
                                                     const void *pointer);
typedef void (APIENTRY *Wc1GlViewportProc)(GLint x, GLint y, GLsizei width,
                                          GLsizei height);

typedef struct Wc1GlFunctions {
    Wc1GlActiveTextureProc ActiveTexture;
    Wc1GlAttachShaderProc AttachShader;
    Wc1GlBindAttribLocationProc BindAttribLocation;
    Wc1GlBindBufferProc BindBuffer;
    Wc1GlBindTextureProc BindTexture;
    Wc1GlBindVertexArrayProc BindVertexArray;
    Wc1GlBlendFuncProc BlendFunc;
    Wc1GlBufferDataProc BufferData;
    Wc1GlClearProc Clear;
    Wc1GlClearColorProc ClearColor;
    Wc1GlCompileShaderProc CompileShader;
    Wc1GlCreateProgramProc CreateProgram;
    Wc1GlCreateShaderProc CreateShader;
    Wc1GlDeleteBuffersProc DeleteBuffers;
    Wc1GlDeleteProgramProc DeleteProgram;
    Wc1GlDeleteShaderProc DeleteShader;
    Wc1GlDeleteTexturesProc DeleteTextures;
    Wc1GlDeleteVertexArraysProc DeleteVertexArrays;
    Wc1GlDisableProc Disable;
    Wc1GlDrawArraysProc DrawArrays;
    Wc1GlEnableProc Enable;
    Wc1GlEnableVertexAttribArrayProc EnableVertexAttribArray;
    Wc1GlGenBuffersProc GenBuffers;
    Wc1GlGenTexturesProc GenTextures;
    Wc1GlGenVertexArraysProc GenVertexArrays;
    Wc1GlGetProgramInfoLogProc GetProgramInfoLog;
    Wc1GlGetProgramivProc GetProgramiv;
    Wc1GlGetShaderInfoLogProc GetShaderInfoLog;
    Wc1GlGetShaderivProc GetShaderiv;
    Wc1GlGetIntegervProc GetIntegerv;
    Wc1GlGetUniformLocationProc GetUniformLocation;
    Wc1GlLinkProgramProc LinkProgram;
    Wc1GlPixelStoreiProc PixelStorei;
    Wc1GlShaderSourceProc ShaderSource;
    Wc1GlTexImage2DProc TexImage2D;
    Wc1GlTexImage3DProc TexImage3D;
    Wc1GlTexParameteriProc TexParameteri;
    Wc1GlTexSubImage3DProc TexSubImage3D;
    Wc1GlUniform1iProc Uniform1i;
    Wc1GlUseProgramProc UseProgram;
    Wc1GlVertexAttribPointerProc VertexAttribPointer;
    Wc1GlViewportProc Viewport;
} Wc1GlFunctions;

typedef struct Wc1GlVertex {
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
} Wc1GlVertex;

typedef struct Wc1GlCachedFrame {
    unsigned char *shape;
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
} Wc1GlCachedFrame;

typedef struct Wc1GlSprite {
    const Wc1GlCachedFrame *cachedFrame;
    short x;
    short y;
    short angle;
    short scale;
    short flip;
    short clipLeft;
    short clipTop;
    short clipRight;
    short clipBottom;
} Wc1GlSprite;

typedef struct Wc1GlAtlasLayer {
    int nextX;
    int nextY;
    int rowHeight;
} Wc1GlAtlasLayer;

typedef enum Wc1GlFrameState {
    WC1_GL_FRAME_IDLE,
    WC1_GL_FRAME_RECORDING,
    WC1_GL_FRAME_COMPLETE
} Wc1GlFrameState;

/* A full world-object list may be followed by four three-part launch doors.
   The title logo uses only three additional entries. */
#define WC1_GL_RECORDED_SPRITE_CAPACITY (WC1_SPACE_OBJECT_COUNT + 12)
#define WC1_GL_SPRITE_VERTEX_COUNT (WC1_GL_RECORDED_SPRITE_CAPACITY * 6)
#define WC1_GL_SPRITE_ATLAS_MAX_SIZE 2048
#define WC1_GL_SPRITE_ATLAS_LAYER_CAPACITY 4
#define WC1_GL_SPRITE_ATLAS_PADDING 1
#define WC1_GL_CACHED_FRAME_CAPACITY 2048

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

typedef struct Wc1GlRenderer {
    SDL_Window *window;
    SDL_GLContext context;
    Wc1GlFunctions gl;
    GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint baseProgram;
    GLuint spriteProgram;
    GLuint baseTexture;
    GLuint paletteTexture;
    GLuint maskTexture;
    GLuint spriteAtlasTexture;
    GLint baseIndicesUniform;
    GLint basePaletteUniform;
    GLint spriteIndicesUniform;
    GLint spritePaletteUniform;
    GLint spriteBaseUniform;
    GLint spriteMaskUniform;
    GLint spriteClearUniform;
    Wc1GlFrameState frameState;
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
    Wc1GlAtlasLayer atlasLayers[WC1_GL_SPRITE_ATLAS_LAYER_CAPACITY];
    unsigned char viewMask[WC1_SDL_FRAME_WIDTH * WC1_SDL_FRAME_HEIGHT];
    unsigned char spaceFrameBase[WC1_SDL_FRAME_WIDTH * WC1_SDL_FRAME_HEIGHT];
    unsigned char presentedFrame[WC1_SDL_FRAME_WIDTH * WC1_SDL_FRAME_HEIGHT];
    unsigned char presentedPalette[256 * 4];
    Wc1GlCachedFrame cachedFrames[WC1_GL_CACHED_FRAME_CAPACITY];
    Wc1GlSprite recordedSprites[WC1_GL_RECORDED_SPRITE_CAPACITY];
    Wc1GlVertex spriteVertices[WC1_GL_SPRITE_VERTEX_COUNT];
} Wc1GlRenderer;

static Wc1GlRenderer g_renderer;

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

    memcpy(destination, pixels, WC1_SDL_FRAME_WIDTH * WC1_SDL_FRAME_HEIGHT);
    if (DAT_0059a84c == 0 || g_stMouseCursorState_0059ab10.viewport == 0 ||
        g_stMouseCursorState_0059ab10.shape == 0 ||
        g_stMouseCursorState_0059ab10.viewport->pixels != pixels)
        return;
    viewport = *g_stMouseCursorState_0059ab10.viewport;
    viewport.pixels = destination;
    viewport.allocation = destination;
    RestoreSpriteBackground(&viewport, DAT_004865a8, (short)DAT_0059a8e4,
                            (short)DAT_0059a8e0,
                            g_stMouseCursorState_0059ab10.shape,
                            (short)g_stMouseCursorState_0059ab10.frame);
}

static int LoadGlFunctions(void)
{
#define WC1_LOAD_GL_FUNCTION(member, type, name)                              \
    do {                                                                      \
        g_renderer.gl.member = (type)SDL_GL_GetProcAddress(name);             \
        if (g_renderer.gl.member == 0) {                                      \
            fprintf(stderr, "OpenGL function %s is unavailable.\n", name);    \
            return 0;                                                         \
        }                                                                     \
    } while (0)

    WC1_LOAD_GL_FUNCTION(ActiveTexture, Wc1GlActiveTextureProc,
                         "glActiveTexture");
    WC1_LOAD_GL_FUNCTION(AttachShader, Wc1GlAttachShaderProc,
                         "glAttachShader");
    WC1_LOAD_GL_FUNCTION(BindAttribLocation, Wc1GlBindAttribLocationProc,
                         "glBindAttribLocation");
    WC1_LOAD_GL_FUNCTION(BindBuffer, Wc1GlBindBufferProc, "glBindBuffer");
    WC1_LOAD_GL_FUNCTION(BindTexture, Wc1GlBindTextureProc, "glBindTexture");
    WC1_LOAD_GL_FUNCTION(BindVertexArray, Wc1GlBindVertexArrayProc,
                         "glBindVertexArray");
    WC1_LOAD_GL_FUNCTION(BlendFunc, Wc1GlBlendFuncProc, "glBlendFunc");
    WC1_LOAD_GL_FUNCTION(BufferData, Wc1GlBufferDataProc, "glBufferData");
    WC1_LOAD_GL_FUNCTION(Clear, Wc1GlClearProc, "glClear");
    WC1_LOAD_GL_FUNCTION(ClearColor, Wc1GlClearColorProc, "glClearColor");
    WC1_LOAD_GL_FUNCTION(CompileShader, Wc1GlCompileShaderProc,
                         "glCompileShader");
    WC1_LOAD_GL_FUNCTION(CreateProgram, Wc1GlCreateProgramProc,
                         "glCreateProgram");
    WC1_LOAD_GL_FUNCTION(CreateShader, Wc1GlCreateShaderProc,
                         "glCreateShader");
    WC1_LOAD_GL_FUNCTION(DeleteBuffers, Wc1GlDeleteBuffersProc,
                         "glDeleteBuffers");
    WC1_LOAD_GL_FUNCTION(DeleteProgram, Wc1GlDeleteProgramProc,
                         "glDeleteProgram");
    WC1_LOAD_GL_FUNCTION(DeleteShader, Wc1GlDeleteShaderProc,
                         "glDeleteShader");
    WC1_LOAD_GL_FUNCTION(DeleteTextures, Wc1GlDeleteTexturesProc,
                         "glDeleteTextures");
    WC1_LOAD_GL_FUNCTION(DeleteVertexArrays, Wc1GlDeleteVertexArraysProc,
                         "glDeleteVertexArrays");
    WC1_LOAD_GL_FUNCTION(Disable, Wc1GlDisableProc, "glDisable");
    WC1_LOAD_GL_FUNCTION(DrawArrays, Wc1GlDrawArraysProc, "glDrawArrays");
    WC1_LOAD_GL_FUNCTION(Enable, Wc1GlEnableProc, "glEnable");
    WC1_LOAD_GL_FUNCTION(EnableVertexAttribArray,
                         Wc1GlEnableVertexAttribArrayProc,
                         "glEnableVertexAttribArray");
    WC1_LOAD_GL_FUNCTION(GenBuffers, Wc1GlGenBuffersProc, "glGenBuffers");
    WC1_LOAD_GL_FUNCTION(GenTextures, Wc1GlGenTexturesProc, "glGenTextures");
    WC1_LOAD_GL_FUNCTION(GenVertexArrays, Wc1GlGenVertexArraysProc,
                         "glGenVertexArrays");
    WC1_LOAD_GL_FUNCTION(GetProgramInfoLog, Wc1GlGetProgramInfoLogProc,
                         "glGetProgramInfoLog");
    WC1_LOAD_GL_FUNCTION(GetProgramiv, Wc1GlGetProgramivProc,
                         "glGetProgramiv");
    WC1_LOAD_GL_FUNCTION(GetShaderInfoLog, Wc1GlGetShaderInfoLogProc,
                         "glGetShaderInfoLog");
    WC1_LOAD_GL_FUNCTION(GetShaderiv, Wc1GlGetShaderivProc, "glGetShaderiv");
    WC1_LOAD_GL_FUNCTION(GetIntegerv, Wc1GlGetIntegervProc, "glGetIntegerv");
    WC1_LOAD_GL_FUNCTION(GetUniformLocation, Wc1GlGetUniformLocationProc,
                         "glGetUniformLocation");
    WC1_LOAD_GL_FUNCTION(LinkProgram, Wc1GlLinkProgramProc, "glLinkProgram");
    WC1_LOAD_GL_FUNCTION(PixelStorei, Wc1GlPixelStoreiProc, "glPixelStorei");
    WC1_LOAD_GL_FUNCTION(ShaderSource, Wc1GlShaderSourceProc,
                         "glShaderSource");
    WC1_LOAD_GL_FUNCTION(TexImage2D, Wc1GlTexImage2DProc, "glTexImage2D");
    WC1_LOAD_GL_FUNCTION(TexImage3D, Wc1GlTexImage3DProc, "glTexImage3D");
    WC1_LOAD_GL_FUNCTION(TexParameteri, Wc1GlTexParameteriProc,
                         "glTexParameteri");
    WC1_LOAD_GL_FUNCTION(TexSubImage3D, Wc1GlTexSubImage3DProc,
                         "glTexSubImage3D");
    WC1_LOAD_GL_FUNCTION(Uniform1i, Wc1GlUniform1iProc, "glUniform1i");
    WC1_LOAD_GL_FUNCTION(UseProgram, Wc1GlUseProgramProc, "glUseProgram");
    WC1_LOAD_GL_FUNCTION(VertexAttribPointer, Wc1GlVertexAttribPointerProc,
                         "glVertexAttribPointer");
    WC1_LOAD_GL_FUNCTION(Viewport, Wc1GlViewportProc, "glViewport");
#undef WC1_LOAD_GL_FUNCTION
    return 1;
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
        log[length < (GLsizei)sizeof(log) ? length : sizeof(log) - 1] = 0;
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
            log[length < (GLsizei)sizeof(log) ? length : sizeof(log) - 1] = 0;
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

static unsigned int GetCachedFrameHash(const unsigned char *shape, short frame,
                                       int dataOffset)
{
    uintptr_t pointerHash;
    unsigned int hash;

    pointerHash = (uintptr_t)shape;
    hash = (unsigned int)(pointerHash ^ (pointerHash >> 16));
    hash ^= (unsigned int)(unsigned short)frame * 0x9e3779b1U;
    hash ^= (unsigned int)dataOffset * 0x85ebca6bU;
    hash ^= hash >> 16;
    return hash & (WC1_GL_CACHED_FRAME_CAPACITY - 1);
}

static Wc1GlCachedFrame *FindCachedFrameSlot(
    unsigned char *shape, short frame, int dataOffset,
    unsigned short shapeTableEnd, int width, int height, short leftExtent,
    short topExtent, int *cacheHit)
{
    Wc1GlCachedFrame *cachedFrame;
    unsigned int slot;
    unsigned int probe;

    slot = GetCachedFrameHash(shape, frame, dataOffset);
    probe = 0;
    while (probe < WC1_GL_CACHED_FRAME_CAPACITY) {
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
        slot = (slot + 1) & (WC1_GL_CACHED_FRAME_CAPACITY - 1);
        probe++;
    }
    return 0;
}

static int ReserveSpriteAtlasRect(int width, int height, short *atlasLayer,
                                  short *atlasLeft, short *atlasTop)
{
    Wc1GlAtlasLayer *layer;
    int packedWidth;
    int packedHeight;
    int x;
    int y;
    int layerIndex;

    packedWidth = width + WC1_GL_SPRITE_ATLAS_PADDING * 2;
    packedHeight = height + WC1_GL_SPRITE_ATLAS_PADDING * 2;
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
            *atlasLayer = (short)layerIndex;
            *atlasLeft = (short)(x + WC1_GL_SPRITE_ATLAS_PADDING);
            *atlasTop = (short)(y + WC1_GL_SPRITE_ATLAS_PADDING);
            layer->nextX = x + packedWidth;
            layer->nextY = y;
            if (x == 0)
                layer->rowHeight = packedHeight;
            else if (packedHeight > layer->rowHeight)
                layer->rowHeight = packedHeight;
            return 1;
        }
        layerIndex++;
    }
    return 0;
}

static const Wc1GlCachedFrame *CacheShapeFrame(
    unsigned char *shape, short frame, int dataOffset,
    unsigned short shapeTableEnd, int width, int height, short leftExtent,
    short topExtent, size_t pixelCount)
{
    Wc1GlCachedFrame *cachedFrame;
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

    packedWidth = width + WC1_GL_SPRITE_ATLAS_PADDING * 2;
    packedHeight = height + WC1_GL_SPRITE_ATLAS_PADDING * 2;
    if (packedWidth > g_renderer.spriteAtlasSize ||
        packedHeight > g_renderer.spriteAtlasSize)
        return 0;
    if (g_renderer.decodeScratchCapacity < pixelCount) {
        resizedScratch =
            (unsigned char *)realloc(g_renderer.decodeScratch, pixelCount);
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
    DecodeShapeFrame(shape, frame, g_renderer.decodeScratch, width, height,
                     leftExtent, topExtent);
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

static void DrawGlQuad(const Wc1GlVertex *vertices)
{
    g_renderer.gl.BindBuffer(GL_ARRAY_BUFFER, g_renderer.vertexBuffer);
    g_renderer.gl.BufferData(GL_ARRAY_BUFFER, sizeof(Wc1GlVertex) * 4,
                             vertices, GL_STREAM_DRAW);
    g_renderer.gl.DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void DrawBaseFrame(void)
{
    const Wc1GlVertex vertices[4] = {
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

static void TransformSpriteVertex(Wc1GlVertex *vertex, float sourceX,
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

static void SetSpriteVertexMetadata(Wc1GlVertex *vertex,
                                    const Wc1GlSprite *sprite,
                                    float outputScale)
{
    const Wc1GlCachedFrame *cachedFrame;

    cachedFrame = sprite->cachedFrame;
    vertex->atlasLeft = (float)cachedFrame->atlasLeft;
    vertex->atlasTop = (float)cachedFrame->atlasTop;
    vertex->atlasWidth = (float)cachedFrame->width;
    vertex->atlasHeight = (float)cachedFrame->height;
    vertex->clipLeft = (float)sprite->clipLeft - 0.5f;
    vertex->clipTop = (float)sprite->clipTop - 0.5f;
    vertex->clipRight = (float)sprite->clipRight + 0.5f;
    vertex->clipBottom = (float)sprite->clipBottom + 0.5f;
    vertex->magnification =
        outputScale *
        (sprite->scale < 0 ? -(float)sprite->scale : (float)sprite->scale) /
        256.0f;
    vertex->atlasLayer = (float)cachedFrame->atlasLayer;
}

static void BuildRecordedSpriteVertices(const Wc1GlSprite *sprite,
                                        float outputScale,
                                        Wc1GlVertex *vertices)
{
    const Wc1GlCachedFrame *cachedFrame;
    Wc1GlVertex corners[4];
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
    GetRLETransformTrig((int)sprite->angle * 10, &fixedCosine, &fixedSine);
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
    right = left + cachedFrame->width;
    bottom = top + cachedFrame->height;
    TransformSpriteVertex(&corners[0], left, top, 0.0f, 0.0f,
                          (float)sprite->x, (float)sprite->y, cosine, sine,
                          scaleX, scaleY);
    TransformSpriteVertex(&corners[1], left, bottom, 0.0f, 1.0f,
                          (float)sprite->x, (float)sprite->y, cosine, sine,
                          scaleX, scaleY);
    TransformSpriteVertex(&corners[2], right, top, 1.0f, 0.0f,
                          (float)sprite->x, (float)sprite->y, cosine, sine,
                          scaleX, scaleY);
    TransformSpriteVertex(&corners[3], right, bottom, 1.0f, 1.0f,
                          (float)sprite->x, (float)sprite->y, cosine, sine,
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
            &g_renderer.spriteVertices[index * 6]);
        index++;
    }
    g_renderer.gl.BindBuffer(GL_ARRAY_BUFFER, g_renderer.vertexBuffer);
    g_renderer.gl.BufferData(
        GL_ARRAY_BUFFER,
        (GLsizeiptr)(sizeof(Wc1GlVertex) * g_renderer.spriteCount * 6),
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
    Wc1SdlCalculateOutputViewport(drawableWidth, drawableHeight, &viewportLeft,
                                  &viewportBottom, &viewportWidth,
                                  &viewportHeight);
    g_renderer.gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    g_renderer.gl.ActiveTexture(GL_TEXTURE0);
    UploadIndexTexture(g_renderer.baseTexture, WC1_SDL_FRAME_WIDTH,
                       WC1_SDL_FRAME_HEIGHT, pixels);
    g_renderer.gl.ActiveTexture(GL_TEXTURE1);
    UploadPaletteTexture(palette);
    if (drawSprites) {
        g_renderer.gl.ActiveTexture(GL_TEXTURE3);
        UploadIndexTexture(g_renderer.maskTexture, WC1_SDL_FRAME_WIDTH,
                           WC1_SDL_FRAME_HEIGHT, g_renderer.viewMask);
    }
    g_renderer.gl.Viewport(0, 0, drawableWidth, drawableHeight);
    g_renderer.gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    g_renderer.gl.Clear(GL_COLOR_BUFFER_BIT);
    g_renderer.gl.Viewport(viewportLeft, viewportBottom, viewportWidth,
                           viewportHeight);
    g_renderer.gl.BindVertexArray(g_renderer.vertexArray);
    DrawBaseFrame();
    if (drawSprites)
        DrawRecordedSprites((float)viewportWidth / WC1_SDL_FRAME_WIDTH);
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
    run = geometry->fadeData;
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
        /* fizzle_fade copies each run with one memcpy into a screen whose
           rows are 320 contiguous bytes, so the count is a byte length and
           a run may cover whole rows: the mode 4 geometry copies 320x128
           as the single run (0, 24, 40960). */
        start = y * WC1_SDL_FRAME_WIDTH + x;
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

int Wc1SdlGlRendererInitialize(SDL_Window *window)
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
        Wc1SdlGlRendererShutdown();
        return 0;
    }
    if (SDL_GL_MakeCurrent(window, g_renderer.context) != 0 ||
        !LoadGlFunctions()) {
        Wc1SdlGlRendererShutdown();
        return 0;
    }
    maxTextureSize = 0;
    maxArrayTextureLayers = 0;
    g_renderer.gl.GetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    g_renderer.gl.GetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,
                              &maxArrayTextureLayers);
    if (maxTextureSize < 1 || maxArrayTextureLayers < 1) {
        fprintf(stderr, "OpenGL texture arrays are unavailable.\n");
        Wc1SdlGlRendererShutdown();
        return 0;
    }
    g_renderer.spriteAtlasSize =
        maxTextureSize < WC1_GL_SPRITE_ATLAS_MAX_SIZE
            ? maxTextureSize
            : WC1_GL_SPRITE_ATLAS_MAX_SIZE;
    g_renderer.spriteAtlasLayerCount =
        maxArrayTextureLayers < WC1_GL_SPRITE_ATLAS_LAYER_CAPACITY
            ? maxArrayTextureLayers
            : WC1_GL_SPRITE_ATLAS_LAYER_CAPACITY;
    g_renderer.baseProgram = LinkGlProgram(g_baseFragmentShaderSource);
    g_renderer.spriteProgram = LinkGlProgram(g_spriteFragmentShaderSource);
    if (g_renderer.baseProgram == 0 || g_renderer.spriteProgram == 0) {
        Wc1SdlGlRendererShutdown();
        return 0;
    }

    g_renderer.gl.GenVertexArrays(1, &g_renderer.vertexArray);
    g_renderer.gl.GenBuffers(1, &g_renderer.vertexBuffer);
    if (g_renderer.vertexArray == 0 || g_renderer.vertexBuffer == 0) {
        fprintf(stderr, "OpenGL renderer buffer creation failed.\n");
        Wc1SdlGlRendererShutdown();
        return 0;
    }
    g_renderer.gl.BindVertexArray(g_renderer.vertexArray);
    g_renderer.gl.BindBuffer(GL_ARRAY_BUFFER, g_renderer.vertexBuffer);
    g_renderer.gl.EnableVertexAttribArray(0);
    g_renderer.gl.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                      sizeof(Wc1GlVertex), 0);
    g_renderer.gl.EnableVertexAttribArray(1);
    g_renderer.gl.VertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(Wc1GlVertex),
        (const void *)(uintptr_t)offsetof(Wc1GlVertex, u));
    g_renderer.gl.EnableVertexAttribArray(2);
    g_renderer.gl.VertexAttribPointer(
        2, 4, GL_FLOAT, GL_FALSE, sizeof(Wc1GlVertex),
        (const void *)(uintptr_t)offsetof(Wc1GlVertex, atlasLeft));
    g_renderer.gl.EnableVertexAttribArray(3);
    g_renderer.gl.VertexAttribPointer(
        3, 4, GL_FLOAT, GL_FALSE, sizeof(Wc1GlVertex),
        (const void *)(uintptr_t)offsetof(Wc1GlVertex, clipLeft));
    g_renderer.gl.EnableVertexAttribArray(4);
    g_renderer.gl.VertexAttribPointer(
        4, 2, GL_FLOAT, GL_FALSE, sizeof(Wc1GlVertex),
        (const void *)(uintptr_t)offsetof(Wc1GlVertex, magnification));

    g_renderer.gl.GenTextures(4, textures);
    g_renderer.baseTexture = textures[0];
    g_renderer.paletteTexture = textures[1];
    g_renderer.maskTexture = textures[2];
    g_renderer.spriteAtlasTexture = textures[3];
    if (g_renderer.baseTexture == 0 || g_renderer.paletteTexture == 0 ||
        g_renderer.maskTexture == 0 ||
        g_renderer.spriteAtlasTexture == 0) {
        fprintf(stderr, "OpenGL renderer texture creation failed.\n");
        Wc1SdlGlRendererShutdown();
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
        Wc1SdlGlRendererShutdown();
        return 0;
    }
    g_renderer.gl.Disable(GL_DITHER);
    SDL_GL_SetSwapInterval(1);
    memset(g_renderer.viewMask, 0xff, sizeof(g_renderer.viewMask));
    return 1;
}

void Wc1SdlGlRendererShutdown(void)
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
    }
    if (g_renderer.context != 0)
        SDL_GL_DeleteContext(g_renderer.context);
    memset(&g_renderer, 0, sizeof(g_renderer));
}

void Wc1SdlGlRendererBeginSpaceFrame(const ScreenViewportGeometry *geometry,
                                     int viewportMode, int fullViewportCopy,
                                     unsigned char spaceClearColour)
{
    ResetRecordedSprites();
    if (g_renderer.atlasResetPending)
        ResetSpriteAtlasCache();
    g_renderer.useSoftwareForRestOfFrame = 0;
    g_renderer.hasLayerSnapshot = 0;
    g_renderer.frameState =
        g_renderer.context != 0 ? WC1_GL_FRAME_RECORDING : WC1_GL_FRAME_IDLE;
    g_renderer.spaceClearColour = spaceClearColour;
    g_renderer.spaceLayerOffsetX = 0;
    g_renderer.spaceLayerOffsetY = 0;
    if (g_renderer.frameState == WC1_GL_FRAME_RECORDING) {
        if (!fullViewportCopy && viewportMode != 5 && geometry != 0) {
            g_renderer.spaceLayerOffsetX = geometry->originX;
            g_renderer.spaceLayerOffsetY = geometry->originY;
        }
        BuildSpaceViewMask(geometry, viewportMode, fullViewportCopy);
    }
}

void Wc1SdlGlRendererCompleteSpaceFrame(void)
{
    if (g_renderer.frameState == WC1_GL_FRAME_RECORDING)
        g_renderer.frameState = WC1_GL_FRAME_COMPLETE;
}

void Wc1SdlGlRendererCancelSpaceFrame(void)
{
    ResetRecordedSprites();
    ResetSpriteAtlasCache();
    g_renderer.frameState = WC1_GL_FRAME_IDLE;
    g_renderer.hasLayerSnapshot = 0;
    g_renderer.spaceLayerOffsetX = 0;
    g_renderer.spaceLayerOffsetY = 0;
    g_renderer.useSoftwareForRestOfFrame = 0;
}

int Wc1SdlGlRendererRecordSpaceSprite(const Viewport *viewport, short x,
                                      short y, unsigned char *shape,
                                      short frame, short angle, short scale,
                                      short flip)
{
    const Wc1GlCachedFrame *cachedFrame;
    Wc1GlSprite *sprite;
    size_t pixelCount;
    int dataOffset;
    int frameTableOffset;
    unsigned short shapeTableEnd;
    short width;
    short height;
    short leftExtent;
    short topExtent;

    if (g_renderer.frameState != WC1_GL_FRAME_RECORDING ||
        g_renderer.useSoftwareForRestOfFrame)
        return 0;
    if (shape == 0 || frame < 0 || scale == 0 || viewport == 0 ||
        viewport->pixels == 0 || viewport->rowOffsets == 0)
        return 0;
    if (flip != 0 && flip != 0x10 && flip != 0x20 && flip != 0x30)
        return 0;
    if (frame >= GetShapeFrameCount(shape))
        return 0;
    if (g_renderer.spriteCount >= WC1_GL_RECORDED_SPRITE_CAPACITY)
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
    memcpy(&shapeTableEnd, shape + 4, sizeof(shapeTableEnd));
    memcpy(&dataOffset, shape + frameTableOffset, sizeof(dataOffset));
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
    sprite->x = (short)(x + g_renderer.spaceLayerOffsetX);
    sprite->y = (short)(y + g_renderer.spaceLayerOffsetY);
    sprite->angle = angle;
    sprite->scale = scale;
    sprite->flip = flip;
    sprite->clipLeft = (short)(viewport->left + g_renderer.spaceLayerOffsetX);
    sprite->clipTop = (short)(viewport->top + g_renderer.spaceLayerOffsetY);
    sprite->clipRight =
        (short)(viewport->right + g_renderer.spaceLayerOffsetX);
    sprite->clipBottom =
        (short)(viewport->bottom + g_renderer.spaceLayerOffsetY);
    g_renderer.spriteCount++;
    return 1;
}

int Wc1SdlGlRendererPresent(const unsigned char *pixels,
                            const unsigned char *palette)
{
    int drawSprites;

    if (pixels == 0 || palette == 0 || g_renderer.context == 0)
        return 0;
    /* Mouse redraws can present the same game frame more than once.  Compare
       cursor-free copies so the recorded object layer survives those redraws
       but is discarded as soon as the underlying game frame changes. */
    drawSprites = 0;
    if (g_renderer.frameState == WC1_GL_FRAME_COMPLETE) {
        CopyBaseWithoutMouse(g_renderer.spaceFrameBase, pixels);
        g_renderer.hasLayerSnapshot = 1;
        g_renderer.frameState = WC1_GL_FRAME_IDLE;
        drawSprites = 1;
    } else if (g_renderer.frameState == WC1_GL_FRAME_IDLE &&
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
    memcpy(g_renderer.presentedPalette, palette,
           sizeof(g_renderer.presentedPalette));
    g_renderer.hasLastFrame = 1;
    return RenderGlFrame(g_renderer.presentedFrame,
                         g_renderer.presentedPalette, drawSprites);
}

void Wc1SdlGlRendererWaitForVerticalBlank(void)
{
    if (!g_renderer.hasLastFrame ||
        !RenderGlFrame(g_renderer.presentedFrame, g_renderer.presentedPalette,
                       g_renderer.hasLayerSnapshot))
        SDL_Delay(1);
}
