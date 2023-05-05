//
// engine.cpp : Put all your graphics stuff in this file. This is kind of the graphics module.
// In here, you should type all your OpenGL commands, and you can also type code to handle
// input platform events (e.g to move the camera or react to certain shortcuts), writing some
// graphics related GUI options, and so on.
//

#include "engine.h"
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "BufferUtilities.h"

#define CreateConstantBuffer(size) CreateBuffer(size, GL_UNIFORM_BUFFER, GL_STREAM_DRAW)
#define CreateStaticVertexBuffer(size) CreateBuffer(size, GL_ARRAY_BUFFER, GL_STATIC_DRAW)
#define CreateStaticIndexBuffer(size) CreateBuffer(size, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW)
#define PushData(buffer, data, size) PushAlignedData(buffer, data, size, 1)
#define PushUInt(buffer, value) { u32 v = value; PushAlignedData(buffer, &v, sizeof(v), 4); }
#define PushVec3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))
#define PushVec4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))
#define PushMat3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))
#define PushMat4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))



bool IsPowerOf2(u32 value)
{
    return value && !(value & (value - 1));
}

u32 Align(u32 value, u32 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}
Buffer CreateBuffer(u32 size, GLenum type, GLenum usage)
{
    Buffer buffer = {};
    buffer.size = size;
    buffer.type = type;

    glGenBuffers(1, &buffer.handle);
    glBindBuffer(type, buffer.handle);
    glBufferData(type, buffer.size, NULL, usage);
    glBindBuffer(type, 0);

    return buffer;
}

void BindBuffer(const Buffer& buffer)
{
    glBindBuffer(buffer.type, buffer.handle);
}

void MapBuffer(Buffer& buffer, GLenum access)
{
    glBindBuffer(buffer.type, buffer.handle);
    buffer.data = (u8*)glMapBuffer(buffer.type, access);
    buffer.head = 0;
}

void UnmapBuffer(Buffer& buffer)
{
    glUnmapBuffer(buffer.type);
    glBindBuffer(buffer.type, 0);
}

void AlignHead(Buffer& buffer, u32 alignment)
{
    ASSERT(IsPowerOf2(alignment), "The alignment must be a power of 2");
    buffer.head = Align(buffer.head, alignment);
}

void PushAlignedData(Buffer& buffer, const void* data, u32 size, u32 alignment)
{
    ASSERT(buffer.data != NULL, "The buffer must be mapped first");
    AlignHead(buffer, alignment);
    memcpy((u8*)buffer.data + buffer.head, data, size);
    buffer.head += size;
}
namespace Utils
{

    u8 GlToShader(GLenum number)
    {
        switch (number)
        {
        case GL_FLOAT_VEC3:
            return 3;
            break;
        case GL_FLOAT_VEC2:
            return 2;
            break;

        default:
            assert("Not implemented yet!");
        }
    }
}



GLuint CreateProgramFromSource(String programSource, const char* shaderName)
{
    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 430\n";
    char shaderNameDefine[128];
    sprintf(shaderNameDefine, "#define %s\n", shaderName);
    char vertexShaderDefine[] = "#define VERTEX\n";
    char fragmentShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        shaderNameDefine,
        vertexShaderDefine,
        programSource.str
    };
    const GLint vertexShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(vertexShaderDefine),
        (GLint) programSource.len
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        shaderNameDefine,
        fragmentShaderDefine,
        programSource.str
    };
    const GLint fragmentShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(fragmentShaderDefine),
        (GLint) programSource.len
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
        assert(success);
      
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
        assert(success);
    }

    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vshader);
    glAttachShader(programHandle, fshader);
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    glUseProgram(0);

    glDetachShader(programHandle, vshader);
    glDetachShader(programHandle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programHandle;
}

u32 LoadProgram(App* app, const char* filepath, const char* programName)
{
    String programSource = ReadTextFile(filepath);

    Program program = {};
    program.handle = CreateProgramFromSource(programSource, programName);
    program.filepath = filepath;
    program.programName = programName;
    program.lastWriteTimestamp = GetFileLastWriteTimestamp(filepath);
   
    GLint attributesCount = 0;
    glGetProgramiv(program.handle, GL_ACTIVE_ATTRIBUTES, &attributesCount);
    for (u32 i = 0; i < attributesCount; ++i)
    {
        char attributeName[90] = {};
        GLsizei attributeSize = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveAttrib(program.handle, i, ARRAY_COUNT(attributeName), &attributeSize, &size, &type, attributeName);

        u8 location = glGetAttribLocation(program.handle, attributeName);
        
        program.vertexInputLayout.attributes.push_back({ location, Utils::GlToShader(type)});

    }


    app->programs.push_back(program);

    return app->programs.size() - 1;
}

Image LoadImage(const char* filename)
{
    Image img = {};
    stbi_set_flip_vertically_on_load(true);
    img.pixels = stbi_load(filename, &img.size.x, &img.size.y, &img.nchannels, 0);
    if (img.pixels)
    {
        img.stride = img.size.x * img.nchannels;
    }
    else
    {
        ELOG("Could not open file %s", filename);
    }
    return img;
}

void FreeImage(Image image)
{
    stbi_image_free(image.pixels);
}

GLuint CreateTexture2DFromImage(Image image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat     = GL_RGB;
    GLenum dataType       = GL_UNSIGNED_BYTE;

    switch (image.nchannels)
    {
        case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
        case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
        default: ELOG("LoadTexture2D() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texHandle;
}

u32 LoadTexture2D(App* app, const char* filepath)
{
    for (u32 texIdx = 0; texIdx < app->textures.size(); ++texIdx)
        if (app->textures[texIdx].filepath == filepath)
            return texIdx;

    Image image = LoadImage(filepath);

    if (image.pixels)
    {
        Texture tex = {};
        tex.handle = CreateTexture2DFromImage(image);
        tex.filepath = filepath;

        u32 texIdx = app->textures.size();
        app->textures.push_back(tex);

        FreeImage(image);
        return texIdx;
    }
    else
    {
        return UINT32_MAX;
    }
}


#pragma region ModelLoad
void ProcessAssimpMesh(const aiScene* scene, aiMesh* mesh, Mesh* myMesh, u32 baseMeshMaterialIndex, std::vector<u32>& submeshMaterialIndices)
{
    std::vector<float> vertices;
    std::vector<u32> indices;

    bool hasTexCoords = false;
    bool hasTangentSpace = false;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            hasTexCoords = true;
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }

        if (mesh->mTangents != nullptr && mesh->mBitangents)
        {
            hasTangentSpace = true;
            vertices.push_back(mesh->mTangents[i].x);
            vertices.push_back(mesh->mTangents[i].y);
            vertices.push_back(mesh->mTangents[i].z);

            // For some reason ASSIMP gives me the bitangents flipped.
            // Maybe it's my fault, but when I generate my own geometry
            // in other files (see the generation of standard assets)
            // and all the bitangents have the orientation I expect,
            // everything works ok.
            // I think that (even if the documentation says the opposite)
            // it returns a left-handed tangent space matrix.
            // SOLUTION: I invert the components of the bitangent here.
            vertices.push_back(-mesh->mBitangents[i].x);
            vertices.push_back(-mesh->mBitangents[i].y);
            vertices.push_back(-mesh->mBitangents[i].z);
        }
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // store the proper (previously proceessed) material for this mesh
    submeshMaterialIndices.push_back(baseMeshMaterialIndex + mesh->mMaterialIndex);

    // create the vertex format
    VertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
    vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
    vertexBufferLayout.stride = 6 * sizeof(float);
    if (hasTexCoords)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 2 * sizeof(float);
    }
    if (hasTangentSpace)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 3, 3, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 3 * sizeof(float);

        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 4, 3, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 3 * sizeof(float);
    }

    // add the submesh into the mesh
    Submesh submesh = {};
    submesh.vertexBufferLayout = vertexBufferLayout;
    submesh.vertices.swap(vertices);
    submesh.indices.swap(indices);
    myMesh->submeshes.push_back(submesh);
}

void ProcessAssimpMaterial(App* app, aiMaterial* material, Material& myMaterial, String directory)
{
    aiString name;
    aiColor3D diffuseColor;
    aiColor3D emissiveColor;
    aiColor3D specularColor;
    ai_real shininess;
    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    material->Get(AI_MATKEY_SHININESS, shininess);

    myMaterial.name = name.C_Str();
    myMaterial.albedo = vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    myMaterial.emissive = vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
    myMaterial.smoothness = shininess / 256.0f;

    aiString aiFilename;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial.albedoTextureIdx = LoadTexture2D(app, filepath.str);
    }
    if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        material->GetTexture(aiTextureType_EMISSIVE, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial.emissiveTextureIdx = LoadTexture2D(app, filepath.str);
    }
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        material->GetTexture(aiTextureType_SPECULAR, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial.specularTextureIdx = LoadTexture2D(app, filepath.str);
    }
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        material->GetTexture(aiTextureType_NORMALS, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial.normalsTextureIdx = LoadTexture2D(app, filepath.str);
    }
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
    {
        material->GetTexture(aiTextureType_HEIGHT, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial.bumpTextureIdx = LoadTexture2D(app, filepath.str);
    }

    //myMaterial.createNormalFromBump();
}

void ProcessAssimpNode(const aiScene* scene, aiNode* node, Mesh* myMesh, u32 baseMeshMaterialIndex, std::vector<u32>& submeshMaterialIndices)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessAssimpMesh(scene, mesh, myMesh, baseMeshMaterialIndex, submeshMaterialIndices);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessAssimpNode(scene, node->mChildren[i], myMesh, baseMeshMaterialIndex, submeshMaterialIndices);
    }
}

u32 LoadModel(App* app, const char* filename)
{
    const aiScene* scene = aiImportFile(filename,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_OptimizeMeshes |
        aiProcess_SortByPType);

    if (!scene)
    {
        ELOG("Error loading mesh %s: %s", filename, aiGetErrorString());
        return UINT32_MAX;
    }

    app->meshes.push_back(Mesh{});
    Mesh& mesh = app->meshes.back();
    u32 meshIdx = (u32)app->meshes.size() - 1u;

    app->models.push_back(Model{});
    Model& model = app->models.back();
    model.meshIdx = meshIdx;
    u32 modelIdx = (u32)app->models.size() - 1u;

    String directory = GetDirectoryPart(MakeString(filename));

    // Create a list of materials
    u32 baseMeshMaterialIndex = (u32)app->materials.size();
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        app->materials.push_back(Material{});
        Material& material = app->materials.back();
        ProcessAssimpMaterial(app, scene->mMaterials[i], material, directory);
    }

    ProcessAssimpNode(scene, scene->mRootNode, &mesh, baseMeshMaterialIndex, model.materialIdx);

    aiReleaseImport(scene);

    u32 vertexBufferSize = 0;
    u32 indexBufferSize = 0;

    for (u32 i = 0; i < mesh.submeshes.size(); ++i)
    {
        vertexBufferSize += mesh.submeshes[i].vertices.size() * sizeof(float);
        indexBufferSize += mesh.submeshes[i].indices.size() * sizeof(u32);
    }

    glGenBuffers(1, &mesh.vertexBufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.indexBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);

    u32 indicesOffset = 0;
    u32 verticesOffset = 0;

    for (u32 i = 0; i < mesh.submeshes.size(); ++i)
    {
        const void* verticesData = mesh.submeshes[i].vertices.data();
        const u32   verticesSize = mesh.submeshes[i].vertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, verticesOffset, verticesSize, verticesData);
        mesh.submeshes[i].vertexOffset = verticesOffset;
        verticesOffset += verticesSize;

        const void* indicesData = mesh.submeshes[i].indices.data();
        const u32   indicesSize = mesh.submeshes[i].indices.size() * sizeof(u32);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indicesData);
        mesh.submeshes[i].indexOffset = indicesOffset;
        indicesOffset += indicesSize;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return modelIdx;
}
#pragma endregion
u32 FindVao(Mesh& mesh, u32 submeshIndex, const Program& program)
{
    Submesh& submesh = mesh.submeshes[submeshIndex];

    for (u32 i = 0; i < (u32)submesh.vaos.size(); ++i)
    {
        if (submesh.vaos[i].programHandle == program.handle)
            return submesh.vaos[i].handle;
    }

    u32 vaoHandle = 0;

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferHandle);

    for (u32 i = 0; i < program.vertexInputLayout.attributes.size(); ++i)
    {
        bool attributeWasLinked = false;

        for (u32 j = 0; j < submesh.vertexBufferLayout.attributes.size(); ++j)
        {
            if (program.vertexInputLayout.attributes[i].location == submesh.vertexBufferLayout.attributes[j].location)
            {
                const u32 index = submesh.vertexBufferLayout.attributes[j].location;
                const u32 ncomp = submesh.vertexBufferLayout.attributes[j].componentCount;
                const u32 offset = submesh.vertexBufferLayout.attributes[j].offset + submesh.vertexOffset;
                const u32 stride = submesh.vertexBufferLayout.stride;

                glVertexAttribPointer(index, ncomp, GL_FLOAT, GL_FALSE, stride, (void*)(u64)offset);
                glEnableVertexAttribArray(index);

                attributeWasLinked = true;
                break;
            }
        }
        assert(attributeWasLinked);
    }

    glBindVertexArray(0);

    Vao vao = { vaoHandle, program.handle };
    submesh.vaos.push_back(vao);

    return vaoHandle;
}

void Init(App* app)
{
    // TODO: Initialize your resources here!
    // - vertex buffers
    // - element/index buffers
    // - vaos
    // - programs (and retrieve uniform indices)
    // - textures

    app->shadingType = ShadingType::FORWARD;
    app->renderTarget = RenderTarget::RENDER_ALBEDO;

    GenerateQuadVao(app);

    app->quadFBshader = LoadProgram(app, "quadFrameBuffer.glsl", "QUAD_FRAMEBUFFER");

    app->quadDeferredShader = LoadProgram(app, "DeferredShader.glsl", "QUAD_DEFERRED");
    
    app->camera = std::make_shared<EditorCamera>(app->displaySize.x, app->displaySize.y, 0.1f, 100.0f);

    // First pass framebuffer
    std::vector<int> attachments = { GL_RGBA16F, GL_RGBA16F, GL_RGBA16F, GL_RGBA16F};
    app->framebuffer = std::make_shared<FrameBuffer>(app->displaySize, attachments);

    // Second pass for the quad and generating the ImGui Image
    attachments = { GL_RGBA16F };
    app->QuadFramebuffer = std::make_shared<FrameBuffer>(app->displaySize, attachments);

    // Get max uniform size allowed for uniform buffers
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &app->maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->uniformBlockAlignment);
    
    // Bind buffer handle   
    app->uniformBuffer = CreateBuffer(app->maxUniformBufferSize, GL_UNIFORM_BUFFER, GL_STATIC_DRAW);
    app->globalParamsOffset = app->uniformBuffer.head;



#pragma region Dices
            // Prepare vertex buffer
    glGenBuffers(1, &app->embeddedVertices);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Prepare index buffer
    glGenBuffers(1, &app->embeddedElements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Prepare Vertex Array Object
    glGenVertexArrays(1, &app->vao);
    glBindVertexArray(app->vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);

    // Layouts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBindVertexArray(0);

    app->texturedGeometryProgramIdx = LoadProgram(app, "shaders.glsl", "TEXTURED_GEOMETRY");
    Program& texturedGeometryProgram = app->programs[app->texturedGeometryProgramIdx];
    app->programUniformTexture = glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");

    app->diceTexIdx = LoadTexture2D(app, "dice.png");
    app->whiteTexIdx = LoadTexture2D(app, "color_white.png");
    app->blackTexIdx = LoadTexture2D(app, "color_black.png");
    app->normalTexIdx = LoadTexture2D(app, "color_normal.png");
    app->magentaTexIdx = LoadTexture2D(app, "color_magenta.png");
#pragma endregion
     
    // ------- Lights -------

    // ------- Directional Lights -------
    Light light;
    light.type = LightType::LightType_Directional;
    light.position = glm::vec3(0.0f, 0.0f, 0.0f);
    light.direction = glm::vec3(-5.0f, 1.0f, -1.0f);
    light.color = glm::vec3(1.0f);

    app->lights.push_back(light);

    Light light2;
    light2.type = LightType::LightType_Directional;
    light2.position = glm::vec3(0.0f, 0.0f, 0.0f);
    light2.direction = glm::vec3(5.0f, 1.0f, -1.0f);
    light2.color = glm::vec3(1.0f);

    app->lights.push_back(light2);

    // ------- Directional Lights End -------


    // ------- Point Lights -------
    Light light3;
    light3.type = LightType::LightType_Point;
    light3.position = glm::vec3(0.0f, 0.0f, 1.0f);
    light3.direction = glm::vec3(1.0f);
    light3.color = glm::vec3(0.4f, 0.0f, 0.4f);

    app->lights.push_back(light3);

    Light light4;
    light4.type = LightType::LightType_Point;
    light4.position = glm::vec3(5.0f, 0.0f, 1.0f);
    light4.direction = glm::vec3(1.0f);
    light4.color = glm::vec3(0.4f, 0.0f, 0.4f);

    app->lights.push_back(light4);

    Light light5;
    light5.type = LightType::LightType_Point;
    light5.position = glm::vec3(-5.0f, 0.0f, 1.0f);
    light5.direction = glm::vec3(1.0f);
    light5.color = glm::vec3(0.4f, 0.0f, 0.4f);

    app->lights.push_back(light5);
    // ------- Point Lights End -------

    // ------- End Lights -------


    // Mesh Program

    // Load model and get model Id, but this Id is for the vector of models, it's not actually the renderer ID!
    //app->model = LoadModel(app, "Patrick/Patrick.obj");
    //u32 model2 = LoadModel(app, "Patrick/Patrick.obj");
    //u32 model3 = LoadModel(app, "Patrick/Patrick.obj");
    app->model = LoadModel(app, "Backpack/backpack.obj");
    u32 model2 = LoadModel(app, "Backpack/backpack.obj");
    u32 model3 = LoadModel(app, "Backpack/backpack.obj");
  
    
    
    Entity ent = {};
    ent.position = vec3(0.0f);
    ent.scale = vec3(1.0f);
    ent.rotation = vec3(0.0f);
    ent.PushEntity(app->model);
    app->entities.push_back(ent);

    Entity ent2 = {};
    ent2.PushEntity(model2);
    ent2.position = vec3(4.0f, 0.0f, 0.0f);
    ent2.scale = vec3(1.0f);
    ent2.rotation = vec3(0.0f);
    app->entities.push_back(ent2);

    Entity ent3 = {};
    ent3.PushEntity(model3);
    ent3.position = vec3(-4.0f, 0.0f, 0.0f);
    ent3.scale = vec3(1.0f);
    ent3.rotation = vec3(0.0f);
    app->entities.push_back(ent3);

    
    // Load shader and get shader Id, but this Id is for the vector of shaders, it's not actually the renderer ID
    app->modelShaderID = LoadProgram(app, "meshShader.glsl", "MESH_GEOMETRY");
    // Get shader itself
    Program& shaderModel = app->programs[app->modelShaderID];
 
    // Load model texture and get texture ID from the vectors of textures.
    app->modelTexture = LoadTexture2D(app, "Backpack/diffuse.jpg");

    // Get uniform location from the texture for later use
    app->modelShaderTextureUniformLocation = glGetUniformLocation(shaderModel.handle, "uTexture");
    
    // End Mesh Program

    app->glInfo.glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    app->glInfo.glRender = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    app->glInfo.glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    app->glInfo.glShadingVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
     
    GLint numExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    for (GLint i = 0; i < numExtensions; ++i)
    {
        app->glInfo.glExtensions.push_back(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, GLuint(i))));
    }

    app->mode = Mode::Mode_Count;
}

void Gui(App* app)
{
    // Enable docking
    ImGui::DockSpaceOverViewport();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Render Targets"))
        {
            if (app->shadingType == ShadingType::FORWARD)
            {
                const char* items[] = { "Albedo", "Normals" , "Position", "Specular", "Depth" };
                static int itemCurrent = 0;
                ImGui::Text("Select Desired:");
                ImGui::Combo("##combo", &itemCurrent, items, IM_ARRAYSIZE(items));
                app->renderTarget = (RenderTarget)itemCurrent;
                ImGui::EndMenu();
            }
            else
            {
                ImGui::Text("Must be in Forward Render Mode to select targets!");
                ImGui::EndMenu();
            }
        }

        if (ImGui::BeginMenu("Render Mode"))
        {
            const char* items[] = { "Forward", "Deferred"};
            static int currentRenderMode = 0;
            ImGui::Text("Select Desired:");
            ImGui::Combo("##combo", &currentRenderMode, items, IM_ARRAYSIZE(items));
            app->shadingType = (ShadingType)currentRenderMode;
            ImGui::EndMenu();
        }
       
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Info");
    ImGui::Text("FPS: %f", 1.0f/app->deltaTime);
    ImGui::End();

    // Show demo window enjain
    //ImGui::ShowDemoWindow();

    ImGui::Begin("Camera Info");
    ImGui::Text("Cam Pos: %f, %f, %f", app->camera->GetPosition().x, app->camera->GetPosition().y, app->camera->GetPosition().z);
    ImGui::End();

    for (u32 i = 0; i < app->entities.size(); ++i)
    {
        ImGui::Begin("Entities Info");
        ImGui::PushID(i);
        ImGui::Text("Entity %d", i);
        glm::vec3& position = app->entities[i].position;
        glm::vec3& scale = app->entities[i].scale;
       
        ImGui::Text("Pos:");
        float windowWidth = ImGui::GetContentRegionAvailWidth();
        ImGui::PushItemWidth(50.0f);
        ImGui::SameLine();
        ImGui::DragFloat("##PosX", &position.x, 0.1f);

        ImGui::SameLine();
        ImGui::DragFloat("##PosY", &position.y, 0.1f);

        ImGui::SameLine();
        ImGui::DragFloat("##PosZ", &position.z, 0.1f);


        ImGui::Text("Rot:");
        glm::vec3 rotation = app->entities[i].rotation;
        rotation = TOANGLE(rotation);
        ImGui::SameLine();
        ImGui::DragFloat("##rotationX", &rotation.x, 0.1f);

        ImGui::SameLine();
        ImGui::DragFloat("##rotationY", &rotation.y, 0.1f);

        ImGui::SameLine();
        ImGui::DragFloat("##rotationZ", &rotation.z, 0.1f);
        app->entities[i].rotation = TORADIANS(rotation);

       
        ImGui::Text("Sca:");
        ImGui::SameLine();
        ImGui::DragFloat("##scaleX", &scale.x, 0.1f);

        ImGui::SameLine();
        ImGui::DragFloat("##scaleY", &scale.y, 0.1f);

        ImGui::SameLine();
        ImGui::DragFloat("##scaleZ", &scale.z, 0.1f);

        ImGui::PopItemWidth();

        ImGui::Separator();


        ImGui::PopID();
        
        ImGui::End();
    }
    for (u32 i = 0; i < app->lights.size(); ++i)
    {
        ImGui::Begin("Lights Info");
        ImGui::PushID(i);

        Light& light = app->lights[i];

        switch (light.type)
        {
        case LightType::LightType_Point:
        {
            ImGui::Text("Light %d (Point)", i);
            ImGui::Text("Pos:");
            float windowWidth = ImGui::GetContentRegionAvailWidth();
            ImGui::PushItemWidth(50.0f);
            ImGui::SameLine();
            ImGui::DragFloat("##PosX", &light.position.x, 0.1f);

            ImGui::SameLine();
            ImGui::DragFloat("##PosY", &light.position.y, 0.1f);

            ImGui::SameLine();
            ImGui::DragFloat("##PosZ", &light.position.z, 0.1f);

            ImGui::PopItemWidth();
            break;
        }
        case LightType::LightType_Directional:
            ImGui::Text("Light %d (Directional)", i);
            break;
        }

        if (ImGui::CollapsingHeader("Color Picker"))
        {
            ImGui::ColorPicker3("Light Color", &light.color[0]);
        }

        ImGui::PopID();
        ImGui::End();
    }
    
    ImGui::Begin("Viewport");
    u32 textureID = app->QuadFramebuffer->colorAttachments[0];
    ImGui::Image((void*)textureID, ImVec2{ (float)app->displaySize.x, (float)app->displaySize.y }, ImVec2{ 0, 1}, ImVec2{ 1, 0 });
    ImGui::End();


    // TODO: Uncomment for OpenGL info.
    //ImGui::OpenPopup("OpenGL Info");
    if (ImGui::BeginPopup("OpenGL Info"))
    {

        ImGui::Text("Version: %s", app->glInfo.glVersion.c_str());
        ImGui::Text("Renderer: %s", app->glInfo.glRender.c_str());
        ImGui::Text("Vendor: %s", app->glInfo.glVendor.c_str());
        ImGui::Text("GLSL Version: %s", app->glInfo.glShadingVersion.c_str());

        ImGui::Separator();
        ImGui::Text("Extensions");

        for (int i = 0; i < app->glInfo.glExtensions.size(); ++i)
        {
            ImGui::Text("Extensions %i : %s", i, app->glInfo.glExtensions[i].c_str());
        }

        ImGui::EndPopup();
    }
    ImGui::CloseCurrentPopup();
}

void Update(App* app)
{
    // Update Camera
    app->camera->Update(app->input, app->deltaTime);

#pragma region Update Uniform buffers
    // ------ Update uniform buffer lights -------
    MapBuffer(app->uniformBuffer, GL_WRITE_ONLY);

    app->globalParamsOffset = app->uniformBuffer.head;

    PushVec3(app->uniformBuffer, app->camera->GetPosition());
    PushUInt(app->uniformBuffer, app->lights.size());

    for (u32 i = 0; i < app->lights.size(); ++i)
    {
        AlignHead(app->uniformBuffer, sizeof(vec4));

        Light& light = app->lights[i];

        PushUInt(app->uniformBuffer, light.type);
        PushVec3(app->uniformBuffer, light.color);
        PushVec3(app->uniformBuffer, light.direction);
        PushVec3(app->uniformBuffer, light.position);
    }

    app->globalParamsSize = app->uniformBuffer.head - app->globalParamsOffset;

    // ------ Update uniform buffer lights End -------


    // ------  Update uniform buffer entities -------

    for (u32 i = 0; i < app->entities.size(); ++i)
    {
        AlignHead(app->uniformBuffer, app->uniformBlockAlignment);

        Entity& entity = app->entities[i];
        glm::mat4 world = entity.GetTransform();
        glm::mat4 mvp = app->camera->GetViewProjection() * entity.GetTransform();
        glm::mat4 view = app->camera->GetView();

        entity.localParamsOffset = app->uniformBuffer.head;
        PushMat4(app->uniformBuffer, world);
        PushMat4(app->uniformBuffer, mvp);
        PushMat4(app->uniformBuffer, view);

        entity.localParamsSize = app->uniformBuffer.head - entity.localParamsOffset;
    }

    UnmapBuffer(app->uniformBuffer);

    // ------ Update uniform buffer entities End -------  
#pragma endregion

}

void Render(App* app)
{
    switch (app->mode)
    {
        case Mode::Mode_TexturedQuad:
            {
                // TODO: Draw your textured quad here!
                // - clear the framebuffer
                // - set the viewport
                // - set the blending state
                // - bind the texture into unit 0
                // - bind the program 
                //   (...and make its texture sample from unit 0)
                // - bind the vao
                // - glDrawElements() !!!

                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glViewport(0, 0, app->displaySize.x, app->displaySize.y);

                Program& programTexturedGeometry = app->programs[app->texturedGeometryProgramIdx];
                glUseProgram(programTexturedGeometry.handle);
                glBindVertexArray(app->vao);

                //glEnable(GL_BLEND);
                //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glUniform1i(app->programUniformTexture, 0);
                glActiveTexture(GL_TEXTURE0);
                GLuint textureHandle = app->textures[app->diceTexIdx].handle;
                glBindTexture(GL_TEXTURE_2D, textureHandle);

                glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);

                glBindVertexArray(0);
                glUseProgram(0);
            }
            break;
        case Mode::Mode_Count:
        {
            glViewport(0, 0, app->displaySize.x, app->displaySize.y);

            // First pass
            // Bind Custom framebuffer
            app->framebuffer->Bind();
 
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Program& shaderModel = app->programs[app->modelShaderID];
            glUseProgram(shaderModel.handle);

            u32 renderModeUniform = glGetUniformLocation(shaderModel.handle, "renderMode");
            glUniform1i(renderModeUniform, (int)app->renderTarget);

            RenderLights(app, shaderModel);
            RenderModels(app, shaderModel);
           
            glUseProgram(0);
            app->framebuffer->Unbind();

            app->QuadFramebuffer->Bind();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            switch (app->shadingType)
            {
            case ShadingType::FORWARD:
                DrawForwardRendering(app);
                break;
            case ShadingType::DEFERRED:
                DrawDeferredRendering(app);
                break;
            default:
                assert((app->shadingType == ShadingType::DEFERRED) || (app->shadingType == ShadingType::FORWARD));
                ELOG("There must be a type of shading method selected!");
            }
            
            DrawQuadVao(app);

            glUseProgram(0);      
            app->QuadFramebuffer->Unbind();
        }
            break;

        default:;
    }
}

void RenderModels(App* app, Program shaderModel)
{
    for (u32 i = 0; i < app->entities.size(); ++i)
    {
        // Bind buffer handle
        glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(1), app->uniformBuffer.handle, app->entities[i].localParamsOffset, app->entities[i].localParamsSize);

        Model& model = app->models[app->entities[i].modelIndex];
        Mesh& mesh = app->meshes[model.meshIdx];

        for (u32 j = 0; j < mesh.submeshes.size(); ++j)
        {
            u32 vao = FindVao(mesh, j, shaderModel);
            glBindVertexArray(vao);

            u32 submeshMaterialIdx = model.materialIdx[j];
            Material& submeshMaterial = app->materials[submeshMaterialIdx];

            glUniform1i(app->modelShaderTextureUniformLocation, 0);
            glActiveTexture(GL_TEXTURE0);
            // This is for Backpack
            GLuint textureHandle = app->textures[app->modelTexture].handle;
            // This for patrick
            //GLuint textureHandle = app->textures[submeshMaterial.albedoTextureIdx].handle;
            glBindTexture(GL_TEXTURE_2D, textureHandle);

            Submesh& submesh = mesh.submeshes[j];
            glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(u64)submesh.indexOffset);
            glBindVertexArray(0);
        }
    }
}

void RenderLights(App* app, Program shaderModel)
{
    // Bind buffer handle
    glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(0), app->uniformBuffer.handle, app->globalParamsOffset, app->globalParamsSize);
}

void GenerateQuadVao(App* app)
{
    glGenVertexArrays(1, &app->quadVao);
    glGenBuffers(1, &app->quadVbo);
   
    glBindVertexArray(app->quadVao);

    glBindBuffer(GL_ARRAY_BUFFER, app->quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void DrawQuadVao(App* app)
{
    glBindVertexArray(app->quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void DrawForwardRendering(App* app)
{
    Program& quadShader = app->programs[app->quadFBshader];
    glUseProgram(quadShader.handle);
    glEnable(GL_BLEND);
    u32 colorLocation = glGetUniformLocation(quadShader.handle, "screenTexture");
    glUniform1i(colorLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    switch (app->renderTarget)
    {
    case RenderTarget::RENDER_ALBEDO:

        glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[0]);
        break;
    case RenderTarget::RENDER_NORMALS:

        glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[1]);
        break;
    case RenderTarget::RENDER_POSITION:

        glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[2]);
        break;
    case RenderTarget::RENDER_SPECULAR:

        glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[3]);
        break;
    case RenderTarget::RENDER_DEPTH:

        glBindTexture(GL_TEXTURE_2D, app->framebuffer->depthAttachmentId);
        break;
    }


    u32 renderLocationUniform = glGetUniformLocation(quadShader.handle, "renderTarget");
    glUniform1i(renderLocationUniform, (int)app->renderTarget);
}

void DrawDeferredRendering(App* app)
{
    glDisable(GL_BLEND);
    Program& quadShader = app->programs[app->quadDeferredShader];
    glUseProgram(quadShader.handle);

    u32 colorLocation = glGetUniformLocation(quadShader.handle, "gColor");
    glUniform1i(colorLocation, 0);

    colorLocation = glGetUniformLocation(quadShader.handle, "gNormal");
    glUniform1i(colorLocation, 1);

    colorLocation = glGetUniformLocation(quadShader.handle, "gPosition");
    glUniform1i(colorLocation, 2);

    colorLocation = glGetUniformLocation(quadShader.handle, "gAlbedoSpec");
    glUniform1i(colorLocation, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[1]);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[2]);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, app->framebuffer->colorAttachments[3]);
}
