//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include <glad/glad.h>
#include "Models.h"
#include "Lights.h"
#include "Camera.h"
#include "FrameBuffer.h"

typedef glm::vec2  vec2;
typedef glm::vec3  vec3;
typedef glm::vec4  vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};

struct Texture
{
    GLuint      handle;
    std::string filepath;
};

struct Program
{
    GLuint             handle;
    std::string        filepath;
    std::string        programName;
    u64                lastWriteTimestamp; // What is this for?
    VertexShaderLayout vertexInputLayout;
};

struct BasicUniformUploader
{
    void UploadUniformInt(Program& shader, const std::string& name, int value)
    {
        GLint location = glGetUniformLocation(shader.handle, name.c_str());
        glUniform1i(location, value);
    }

    void UploadUniformFloat(Program& shader, const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(shader.handle, name.c_str());
        glUniform1f(location, value);
    }

    void UploadUniformFloat2(Program& shader, const std::string& name, const glm::vec2& values)
    {
        GLint location = glGetUniformLocation(shader.handle, name.c_str());
        glUniform2f(location, values.x, values.y);
    }

    void UploadUniformFloat3(Program& shader, const std::string& name, const glm::vec3& values)
    {
        GLint location = glGetUniformLocation(shader.handle, name.c_str());
        glUniform3f(location, values.x, values.y, values.z);
    }

    void UploadUniformFloat4(Program& shader, const std::string& name, const glm::vec4& values)
    {
        GLint location = glGetUniformLocation(shader.handle, name.c_str());
        glUniform4f(location, values.x, values.y, values.z, values.w);
    }

    void UploadUniformMat3(Program& shader, const std::string& name, const glm::mat3& matrix)
    {
        GLint location = glGetUniformLocation(shader.handle, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void UploadUniformMat4(Program& shader, const std::string& name, const glm::mat4& matrix)
    {
        GLint location = glGetUniformLocation(shader.handle, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
};

enum class Mode
{
    Mode_TexturedQuad,
    Mode_Count
};

struct OpenGLInfo
{
    std::string                 glVersion;
    std::string                 glRender;
    std::string                 glVendor;
    std::string                 glShadingVersion;
    std::vector<std::string>    glExtensions;
};


struct Buffer
{
    u32 size;
    GLenum type;
    u32 handle;
    void* data;
    u32 head;
};

struct VertexV3V2
{
    glm::vec3 pos;
    glm::vec2 uv;
};

const VertexV3V2 vertices[] =
{
    {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)},
    {glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},
    {glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 1.0f)},
    {glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 1.0f)}
};

const u16 indices[] =
{
    0, 1, 2,
    0, 2, 3
};

const float quadVertices[]
{
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

enum class RenderTarget
{
    RENDER_ALBEDO = 0,
    RENDER_NORMALS,
    RENDER_POSITION,
    RENDER_SPECULAR,
    RENDER_DEPTH
};

enum class ShadingType
{
    FORWARD = 0,
    DEFERRED
};

struct App
{
    // Loop
    f32  deltaTime;
    bool isRunning;

    // Input
    Input input;
    bool canUseInput = true;

    // Graphics
    char gpuName[64];
    char openGlVersion[64];

    ivec2 displaySize;

    // Forward and Deferred
    ShadingType shadingType;

    // Camera
    std::shared_ptr<EditorCamera> camera;

    // Framebuffers
    std::shared_ptr<FrameBuffer> framebuffer;
    std::shared_ptr<FrameBuffer> QuadFramebuffer;

    // Framebuffer image shader
    u32 quadFBshader;
    u32 quadDeferredShader;
    u32 quadVao;
    u32 quadVbo;

    // Meshes and materials
    std::vector<Texture>  textures;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Model> models;
    std::vector<Program>  programs;

    // Model test
    u32 model;
    u32 modelShaderID;
    u32 modelShaderTextureUniformLocation;
    u32 modelShaderNormalTextureUniformLocation;
    u32 modelShaderDepthTextureUniformLocation;
    u32 modelTexture;
    u32 modelNormalTexture;
    u32 modelDepthTexture;

    bool useNormal = true;
    bool useDepth = true;

    // Uniforms helper
    BasicUniformUploader uniformUploader;

    // Entities
    std::vector<Entity> entities;

    // Lights
    std::vector<Light> lights;
    u32 lightShader;
    bool activeLights = true;

    // --------- Uniform buffers mesh Shader ---------
    // Uniform buffers size and alignment
    i32 maxUniformBufferSize;
    i32 uniformBlockAlignment;

    // Local Uniform buffers
    Buffer uniformBuffer;

    // Global uniform buffer
    u32 globalParamsOffset;
    u32 globalParamsSize;
    // --------- Uniform buffers mesh Shader end ---------

    // program indices
    u32 texturedGeometryProgramIdx;
    
    // texture indices
    u32 diceTexIdx;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;

    //Render Target
    RenderTarget renderTarget;

    // Mode
    Mode mode;

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint embeddedVertices;
    GLuint embeddedElements;

    // Location of the texture uniform in the textured quad shader
    GLuint programUniformTexture;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint vao;

    // OpenGL info
    OpenGLInfo glInfo;
};

void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

void RenderModels(App* app, Program shaderModel);
void RenderLights(App* app, Program shaderModel, bool active);

void GenerateQuadVao(App* app);

void DrawQuadVao(App* app);

void DrawForwardRendering(App* app);

void DrawDeferredRendering(App* app);

