/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gles3jni.h"
#include <EGL/egl.h>

#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <math.h>
#include "glm/glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.inl>
#include "glm/mat4x4.hpp"
#include "glm/matrix.hpp"

#include "Vertices.h"


#define STR(s) #s
#define STRV(s) STR(s)

#define POS_ATTRIB 0
#define COLOR_ATTRIB 1
#define SCALEROT_ATTRIB 2
#define OFFSET_ATTRIB 3

static const char VERTEX_SHADER_BAK[] =
    "#version 300 es\n"
    "layout(location = " STRV(POS_ATTRIB) ") in vec3 pos;\n"
    "layout(location=" STRV(COLOR_ATTRIB) ") in vec2 color;\n"
    "layout(location=" STRV(SCALEROT_ATTRIB) ") in vec4 scaleRot;\n"
    "layout(location=" STRV(OFFSET_ATTRIB) ") in vec2 offset;\n"
    "out vec2 vTexCood;\n"

    "uniform mat4 mvp_mat;\n"
    "void main() {\n"
    "    gl_Position = mvp_mat * vec4(pos, 1.0);\n"
    "    vTexCood = color;\n"
    "}\n";

static const char VERTEX_SHADER[] =
        "#version 300 es\n"
        "layout(location = " STRV(POS_ATTRIB) ") in vec3 pos;\n"
        "layout(location=" STRV(COLOR_ATTRIB) ") in vec2 color;\n"
                   "layout(location = 4) in vec3 normal;\n"
        "out vec2 vTexCood;\n"
        "out vec4 v_world_pos;\n"
        "out vec3 v_normal;\n"
        "uniform mat4 mvp_mat;\n"
        "void main() {\n"
        "    gl_Position = mvp_mat * vec4(pos, 1.0);\n"
        "    v_world_pos = vec4(pos, 1.0);\n"
        "    v_normal = normal;\n"
        "    vTexCood = color;\n"
        "}\n";


static const char FRAGMENT_SHADER[] =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 vTexCood;\n"
        "in vec4 v_world_pos;\n"
        "in vec3 v_normal;\n"
        "out vec4 outColor;\n"
        "uniform sampler2D texture0;\n"
        ""
        "#define PI 3.14159265\n"
        ""
        "vec3 eye_pos = vec3(0.0, 0.0, 1.0);\n"
        "vec3 light = vec3(1.0, 1.0, 1.0);\n"
        "vec3 light_pos = vec3(1.0, 0.5, 2.0);\n"
        "float alpha = 0.4;\n"
        "vec3 f0 = vec3(0.56, 0.57, 0.58);\n"
        ""
        "float geometry_ggx(float ndotv, float k) {\n"
        "   float denom = ndotv * (1.0 - k) + k;\n"
        "   return ndotv / denom;\n"
        "   }\n"
        ""
        "vec3 fresnel(float hdotv, vec3 f0) {\n"
        "   return f0 + (1.0 - f0) * pow((1.0 - hdotv), 1.0);\n"
        "   }\n"
        ""
        ""
        "void main() {\n"
        "   vec3 albedo = texture(texture0, vTexCood).rgb;\n"
        "   vec3 ambient = albedo * vec3(0.1);\n"
        ""
        "    vec3 n = v_normal;\n"
        ""
        //        " if (!gl_FrontFacing)   n = -n;\n"
        ""
        "    vec3 l = normalize(light_pos - v_world_pos.xyz);\n"
        "    vec3 v = normalize(eye_pos - v_world_pos.xyz);\n"
        "    vec3 h = normalize(l + v);\n"
        "    float n_dot_h = dot(n , h);\n"
        ""
        //        "    vec3 specular = light * pow(dot(n , h), 20.0);\n"
        ""
        "    float D = alpha * alpha / (PI * pow((n_dot_h * n_dot_h * (alpha * alpha - 1.0) + 1.0), 2.0) );\n"

        "   float k = (alpha + 1.0) * (alpha + 1.0) / 8.0;\n"
        "   float ggxv = geometry_ggx(max(dot(n, v), 0.0), k);\n "
        "   float ggxl = geometry_ggx(max(dot(n, l), 0.0), k);\n "
        "   float G = ggxl * ggxv;\n"

        "   vec3 F = fresnel(max(dot(h, v), 0.0), f0);\n"

        "    float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0);\n"
        "    denominator = max(denominator, 0.001);\n"
        "    vec3 func_kc =  D * G * F / denominator;\n"
        "    vec3 brdf = (1.0 - func_kc) * albedo / PI + func_kc;\n"
        "    vec3 final_color = brdf * light * max(dot(n, l), 0.0) + ambient;\n"
        "    outColor = vec4(final_color, 1.0);\n"

        "}\n";

static const char FRAGMENT_PBR_SHADER[] =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 outColor;\n"
        "in vec2 vTexCood;\n"
        "uniform sampler2D texture0;\n"
        "void main() {\n"
//        "outColor = texture(texture0, vTexCood);\n"
        "float ;\n"
        "outColor.a = outColor.a * (1.0f - depth);\n"
        "}\n";


static const char FRAGMENT_SHADER_DOF[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "out vec4 outColor;\n"
                "in vec2 vTexCood;\n"
                "uniform sampler2D texture0;\n"
                "uniform sampler2D texture1;\n"
                ""
                "vec4 uniformDistribution(float c, sampler2D texture_orig, vec2 tex_c) {\n"
                "    if (c < 0.0000001) { return texture(texture_orig, tex_c); }\n"
                "    float r = c / 2.0f;\n"
                "    float a = 4.0 * r * r;\n"
                "    vec4 dis_color;\n"
                "    float step = 1.0f / 1280.0f;\n"
                "    //vec4 intensity = texture(texture_orig, tex_c) / a;\n"
                "    for (float row = -step * r; row <= step * r; row += step) {\n"
                "        for (float col = -step * r; col <= step * r; col += step) {\n"
                "            dis_color += texture(texture_orig, vec2(tex_c.x + row, tex_c.y + col));\n"
                "        }\n"
                "    }\n"
                "    dis_color /= a;\n"
                "    return dis_color;\n"
                "}\n"
                ""
                ""
                "void main() {\n"
                "//outColor = texture(texture0, vTexCood);\n"
                "float depth = texture(texture1, vTexCood).r;\n"
                "float real_z = (0.1 * 100.0) / (100.0 - depth * (100.0 - 0.1));\n"
                ""
                "float c = 2.0 * abs(5.0 * (10.0 - 1.0 / real_z) - 1.0);\n"
                "c = c > 18.0f ? 18.0f : c;\n"
                "    float r = c / 2.0f;\n"
                "    float even_r = roundEven(r);\n"
                ""
                "    vec4 dis_color = vec4(0, 0, 0, 1.0);\n"
                "    float step = 1.0f / 1280.0f;\n"
                "    float index = 0.0;\n"
                "    for (float row = -even_r; row <= even_r; row += 2.0) {\n"
                "        for (float col = -even_r; col <= even_r; col += 2.0) {\n"
                "dis_color += texture(texture0, vec2(vTexCood.x + col * step, vTexCood.y + row * step));\n"
                "index++;\n"
                "        }\n"
                "    }\n"
                "    dis_color /= index;\n"
                "    outColor = dis_color;\n"                ""
                ""
                ""
                "}\n";

/*static const float TEX_COORD[] = {
        0, 1,
        1, 1,
        0, 0,
        1, 0
};*/

static const float VERTICS[] = {
        -0.5f, -0.5f, -1.0f,
        0.5f, -0.5f, -1.0f,
        -0.5f, 0.5f, -1.0f,
        0.5f, 0.5f, -1.0f
};
static const float TEX_COORD[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
};

/*
 *                 "    for (float row = -1.0; row <= 1.0; row += 1.0) {\n"
                "        for (float col = -1.0; col <= 1.0; col += 1.0) {\n"
                "            dis_color += texture(texture0, vec2(vTexCood.x + row / step,"
                "                vTexCood.y + col / step));\n"
                "        }\n"
                "    }\n"
 */

/*                "    dis_color += texture(texture0, vec2(vTexCood.x, vTexCood.y + step));\n"
                "    dis_color += texture(texture0, vec2(vTexCood.x, vTexCood.y - step));\n"
                "    dis_color += texture(texture0, vec2(vTexCood.x, vTexCood.y));\n"
                "    dis_color += texture(texture0, vec2(vTexCood.x + step, vTexCood.y));\n"
                "    dis_color += texture(texture0, vec2(vTexCood.x - step, vTexCood.y));\n"*/

class RendererES3: public Renderer {
public:
    RendererES3();
    virtual ~RendererES3();
    bool init();

    void set2DTexture(uint32_t *data, int width, int height) override;

    void setDepthTexture(uint32_t *data, int width, int height) override;

    void resize(int w, int h) override;

private:
    enum {VB_INSTANCE, VB_SCALEROT, VB_OFFSET, VB_COUNT};

    virtual float* mapOffsetBuf();
    virtual void unmapOffsetBuf();
    virtual float* mapTransformBuf();
    virtual void unmapTransformBuf();
    virtual void draw(unsigned int numInstances);

    const EGLContext mEglContext;
    GLuint mProgram;
    GLuint mVB[VB_COUNT];
    GLuint mVBState;
};

Renderer* createES3Renderer() {
    RendererES3* renderer = new RendererES3;
    if (!renderer->init()) {
        delete renderer;
        return NULL;
    }
    return renderer;
}

RendererES3::RendererES3()
:   mEglContext(eglGetCurrentContext()),
    mProgram(0),
    mVBState(0)
{
    for (int i = 0; i < VB_COUNT; i++)
        mVB[i] = 0;
}

bool RendererES3::init() {
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgram)
        return false;

/*    glGenBuffers(VB_COUNT, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD), &QUAD[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * 4*sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_OFFSET]);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * 2*sizeof(float), NULL, GL_STATIC_DRAW);*/

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * CUBE_VERTIC_NUM * 6, &CUBE_VERTICES[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    // Position
    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
//    glVertexAttribPointer(POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
    glVertexAttribPointer(POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const GLvoid*)0);
    glEnableVertexAttribArray(POS_ATTRIB);

    // Normal
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(4);

    // Texture Coordinates
    GLuint color_buf;
    glGenBuffers(1, &color_buf);
    glBindBuffer(GL_ARRAY_BUFFER, color_buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * CUBE_VERTIC_NUM * 2, CUBE_TEX_COORD, GL_STATIC_DRAW);

    glVertexAttribPointer(COLOR_ATTRIB, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(COLOR_ATTRIB);

    checkGlError("Init()2");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    ALOGV("Using OpenGL ES 3.0 renderer");

    checkGlError("Init()");
    return true;
}

RendererES3::~RendererES3() {
    /* The destructor may be called after the context has already been
     * destroyed, in which case our objects have already been destroyed.
     *
     * If the context exists, it must be current. This only happens when we're
     * cleaning up after a failed init().
     */
    if (eglGetCurrentContext() != mEglContext)
        return;
    glDeleteVertexArrays(1, &mVBState);
    glDeleteBuffers(VB_COUNT, mVB);
    glDeleteProgram(mProgram);
}

float* RendererES3::mapOffsetBuf() {
    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_OFFSET]);
    return (float*)glMapBufferRange(GL_ARRAY_BUFFER,
            0, MAX_INSTANCES * 2*sizeof(float),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

void RendererES3::unmapOffsetBuf() {
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

float* RendererES3::mapTransformBuf() {
    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
    return (float*)glMapBufferRange(GL_ARRAY_BUFFER,
            0, MAX_INSTANCES * 4*sizeof(float),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

void RendererES3::unmapTransformBuf() {
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void RendererES3::draw(unsigned int numInstances) {
    glUseProgram(mProgram);
    glBindVertexArray(mVBState);
//    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numInstances);
    glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTIC_NUM);
}


void RendererES3::set2DTexture(uint32_t *data, int width, int height) {
    Renderer::set2DTexture(data, width, height);

    glUseProgram(mProgram);

    GLuint texture;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0 ,GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUniform1i(glGetUniformLocation(mProgram, "texture0"), 0);
}


void RendererES3::setDepthTexture(uint32_t *data, int width, int height) {
    Renderer::setDepthTexture(data, width, height);

    glUseProgram(mProgram);

    GLuint texture;

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0 ,GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glUniform1i(glGetUniformLocation(mProgram, "texture1"), 1);
}

void RendererES3::resize(int w, int h) {
    Renderer::resize(w, h);

    glUseProgram(mProgram);

    // Uniforms
    glm::vec3 eye_pos = glm::vec3(1.0, 0.8, 2.0);
    glm::vec3 center_point = eye_pos * -1.0f;
    glm::mat4 view_mat = glm::lookAt(eye_pos, center_point, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 project_mat = glm::perspective((float)(1.0f * M_PI_4), (w * 1.0f / h * 1.0f), 0.01f, 10.0f);

    glm::mat4 mvp_mat = project_mat * view_mat;

//    ALOGE("%f", mvp_mat[0][0]);
//    ALOGE("location %d", glGetUniformLocation(mProgram, "mvp_mat"));
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "mvp_mat"), 1, GL_FALSE, glm::value_ptr(mvp_mat));
    checkGlError("resize");
}
