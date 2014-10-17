/****************************************************************************
**
** Copyright (C) 2014 Cutehacks AS.
** Contact: http://www.cutehacks.com/contact
**
****************************************************************************/

#include "scenegraph.h"
#include "mathematics.h"
#include <stdlib.h>

using namespace SceneGraph;

// API

static const float identity_matrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1 };

State::State()
{
    reset();
}

State::~State()
{
    clearMatrices();
}

void State::clearMatrices()
{
    while (!m_matrices.empty()) {
        delete m_matrices.top();
        m_matrices.pop();
    }
}

void State::reset()
{
    clearMatrices();
    m_matrices.push(new float[16]);
    memcpy(m_matrices.top(), identity_matrix, sizeof(float) * 16);
    setProjectionMatrix(identity_matrix);
}

void State::execute(Node *node)
{
    if (node->enabled(this)) {
        node->prepare(this);
        node->update(this);
        node->execute(this);
        if (node->visible(this)) {
            std::list<Node*>::const_iterator it = node->m_children.begin();
            for (; it != node->m_children.end(); ++it)
                execute(*it);
        }
        node->cleanup(this);
    }
}

void State::pushMatrix()
{
    float *matrix = new float[16];
    memcpy(matrix, m_matrices.top(), sizeof(float) * 16);
    m_matrices.push(matrix);
}

void State::pushIdentityMatrix()
{
    float *matrix = new float[16];
    memcpy(matrix, identity_matrix, sizeof(float) * 16);
    m_matrices.push(matrix);
}

void State::multiplyMatrix(const float *matrix)
{
    static float result[16];
    multiply_matrices(m_matrices.top(), matrix, result);
    memcpy(m_matrices.top(), result, sizeof(float) * 16);
}

void State::popMatrix()
{
    delete m_matrices.top();
    m_matrices.pop();
}

const float *State::currentMatrix()
{
    return m_matrices.top();
}

void State::setOrtographicProjection(float left, float right,
                                     float bottom, float top,
                                     float near, float far)
{
    float m[] = {
        2 / (right - left), 0, 0, -((right + left) / (right - left)),
        0, 2 / (top - bottom), 0, -((top + bottom) / (top - bottom)),
        0, 0, -2 / (far - near), -((far + near) / (far - near)),
        0, 0, 0, 1
    };
    setProjectionMatrix(m);
}

void State::setOrtographicProjectionEx(float aspect, float fov_y,
                                       float near, float far)
{
    float top = -near * tanf(fov_y / 2.0); // NOTE: reverse negative
    float right = aspect * top;
    setOrtographicProjection(-right, right, top, -top, near, far);
}

void State::setPerspectiveProjection(float left, float right,
                                     float bottom, float top,
                                     float near, float far)
{
    float m[] = {
        (2 * near) / (right - left), 0, (right + left) / (right - left), 0,
        0, (2 * near) / (top - bottom), (top + bottom) / (top - bottom), 0,
        0, 0, -((far + near) / (far - near)), -((2 * far * near) / (far - near)),
        0, 0, -1, 0
    };
    setProjectionMatrix(m);
}

void State::setPerspectiveProjectionEx(float aspect, float fov_y,
                                       float near, float far)
{
    float top = near * tanf(fov_y / 2.0);
    float right = aspect * top;
    setPerspectiveProjection(-right, right, top, -top, near, far);
}

void State::setProjectionMatrix(const float *matrix)
{
    memcpy(m_projection_matrix, matrix, sizeof(float) * 16);
}

const float *State::projectionMatrix()
{
    return State::m_projection_matrix;
}

// Node

Node::Node(Node *parent)
    : m_parent(parent)
{
    if (parent)
        parent->m_children.push_back(this);
}

Node::~Node()
{
    if (m_parent)
        m_parent->m_children.remove(this);
    std::list<Node*>::iterator it = m_children.begin();
    for (; it != m_children.end(); ++it) {
        (*it)->m_parent = 0;
        delete (*it);
    }
}

Node *Node::parent() const
{
    return m_parent;
}

std::list<Node*> Node::children() const
{
    return m_children;
}

bool Node::enabled(State *)
{
    return true;
}

void Node::prepare(State *)
{
}

void Node::execute(State *)
{
}

bool Node::visible(State *)
{
    return true;
}

void Node::cleanup(State *)
{
}

void Node::update(State *)
{
}

// Transformation

Transformation::Transformation(const float *matrix, Node *parent)
    : Node(parent)
{
    setMatrix(matrix);
}

Transformation::~Transformation()
{
}

void Transformation::prepare(State *state)
{
    state->pushMatrix();
}

void Transformation::execute(State *state)
{
    state->multiplyMatrix(m_matrix);
}

void Transformation::cleanup(State *state)
{
    state->popMatrix();
}

void Transformation::translate(float dx, float dy, float dz)
{
    const float translation_matrix[] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        dx, dy, dz, 1.0
    };
    multiply(translation_matrix);
}

void Transformation::scale(float sx, float sy, float sz)
{
    const float scale_matrix[] = {
        sx, 0.0, 0.0, 0.0,
        0.0, sy, 0.0, 0.0,
        0.0, 0.0, sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    multiply(scale_matrix);
}

void Transformation::create_rotation_matrix(float vx, float vy, float vz, float radians, float *destination)
{
    float quaternion[] = {
        cosf(radians / 2.0),
        vx * sinf(radians / 2.0),
        vy * sinf(radians / 2.0),
        vz * sinf(radians / 2.0) };
    quaternion_rotation_matrix(quaternion, destination);
}

void Transformation::rotate(float vx, float vy, float vz, float radians)
{
    // NOTE: this operation will cause the scale to drift
    float rotation_matrix[16];
    create_rotation_matrix(vx, vy, vz, radians, rotation_matrix);
    multiply(rotation_matrix);
}

void Transformation::multiply(const float *transformation)
{
    float transformed_matrix[16];
    multiply_matrices(m_matrix, transformation, transformed_matrix);
    setMatrix(transformed_matrix);
}

float *Transformation::matrix()
{
    return m_matrix;
}

void Transformation::setMatrix(const float *matrix)
{
    if (!matrix)
        matrix = identity_matrix;
    memcpy(m_matrix, matrix, sizeof(float) * 16);
}

// Shader

const char *Shader::default_vertex_shader =
#ifdef GL_ES_VERSION_2_0
        "#version 100\n"
#endif
        "uniform mat4 sg_projection_model_view_matrix;\n"
        "attribute vec3 sg_position_attribute;\n"
        "attribute vec2 sg_texuv_attribute;\n"
        "varying vec2 v_texuv;\n"
        "void main()\n"
        "{\n"
            "gl_Position = sg_projection_model_view_matrix * vec4(sg_position_attribute, 1.0);\n"
            "v_texuv = vec2(sg_texuv_attribute);\n"
        "}";
const char *Shader::default_fragment_shader =
#ifdef GL_ES_VERSION_2_0
        "#version 100\n"
        "precision highp float;\n"
        "uniform highp sampler2D sg_texture_sampler;\n"
#else
        "uniform sampler2D sg_texture_sampler;\n"
#endif
        "varying vec2 v_texuv;\n"
        "void main()\n"
        "{\n"
            "gl_FragColor = texture2D(sg_texture_sampler, v_texuv);\n"
        "}";

const char *Shader::projection_model_view_matrix_uniform_name = "sg_projection_model_view_matrix";
const char *Shader::texture_sampler_uniform_name = "sg_texture_sampler";
const char *Shader::position_attribute_name = "sg_position_attribute";
const char *Shader::normal_attribute_name = "sg_normal_attribute";
const char *Shader::texuv_attribute_name = "sg_texuv_attribute";

Shader::Shader(const char *vertex_source,
               const char *fragment_source,
               unsigned int uniforms,
               unsigned int attributes,
               Node *parent)
    : Node(parent),
      m_program(0),
      m_old_program(0),
      m_owner(false)
{
    initialize(vertex_source, fragment_source, uniforms, attributes);
}

Shader::Shader(Shader *other, Node *parent)
    : Node(parent),
      m_program(other ? other->m_program : 0),
      m_old_program(0),
      m_uniforms(other ? other->m_uniforms : 0),
      m_owner(false)
{
}

Shader::~Shader()
{
    if (m_owner)
        glDeleteProgram(m_program);
}

bool Shader::initialize(const char *vertex_source,
                        const char *fragment_source,
                        unsigned int uniforms,
                        unsigned int attributes)
{
    m_owner = true;
    m_uniforms = uniforms;

    GLint compile_ok = GL_FALSE;
    GLint link_ok = GL_FALSE;
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;

    if (vertex_source) {
       vertex_shader = glCreateShader(GL_VERTEX_SHADER);
       glShaderSource(vertex_shader, 1, &vertex_source, 0);
       glCompileShader(vertex_shader);
       glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_ok);
       if (!compile_ok) {
           GLint logLen;
           glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logLen);
           if (logLen > 0) {
               GLchar *log = (GLchar *)malloc(logLen);
               glGetShaderInfoLog(vertex_shader, logLen, &logLen, log);
               fprintf(stderr, "Vertex shader info log: %s\n", log);
               free(log);
           }
       }
    }

    if (fragment_source) {
       fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
       glShaderSource(fragment_shader, 1, &fragment_source, 0);
       glCompileShader(fragment_shader);
       glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_ok);
       if (!compile_ok) {
         GLint logLen;
         glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logLen);
         if (logLen > 0) {
             GLchar *log = (GLchar *)malloc(logLen);
             glGetShaderInfoLog(fragment_shader, logLen, &logLen, log);
             fprintf(stderr, "Fragment shader info log: %s\n", log);
             free(log);
         }
       }
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex_shader);
    glAttachShader(m_program, fragment_shader);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        GLint logLen;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            GLchar *log = (GLchar *)malloc(logLen);
            glGetProgramInfoLog(m_program, logLen, &logLen, log);
            fprintf(stderr, "Shader program info log: %s\n", log);
            free(log);
        }
    }

    glValidateProgram(m_program);

    if (attributes & PositionAttribute) {
        GLint position_attrib = glGetAttribLocation(m_program, Shader::position_attribute_name);
        if (position_attrib == -1)
            fprintf(stderr, "Could not bind attribute %s\n", Shader::position_attribute_name);
    }

    if (attributes & NormalAttribute) {
        GLint normal_attrib = glGetAttribLocation(m_program, Shader::normal_attribute_name);
        if (normal_attrib == -1)
            fprintf(stderr, "Could not bind attribute %s\n", Shader::normal_attribute_name);
    }

    if (attributes & TexuvAttribute) {
        GLint texuv_attrib = glGetAttribLocation(m_program, Shader::texuv_attribute_name);
        if (texuv_attrib == -1)
            fprintf(stderr, "Could not bind attribute %s\n", Shader::texuv_attribute_name);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (!compile_ok || !link_ok) {
        glDeleteProgram(m_program);
        m_program = 0;
        m_owner = false;
        return false;
    }

    return true;
}

void Shader::prepare(State *)
{
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&m_old_program);
}

void Shader::execute(State *state)
{
    if (m_old_program != m_program)
        glUseProgram(m_program);
    if (m_uniforms & ProjectionModelViewUniform) {
        float mvp_matrix[16];
        multiply_matrices(state->projectionMatrix(), state->currentMatrix(), mvp_matrix);
        setUniformMatrix4fv(Shader::projection_model_view_matrix_uniform_name, mvp_matrix, false);
    }
    if (m_uniforms & TextureSamplerUniform) {
        setUniform1i(Shader::texture_sampler_uniform_name, /*GL_TEXTURE*/0);
    }
}

void Shader::cleanup(State *)
{
    if (m_old_program != m_program)
        glUseProgram(m_old_program);
}

bool Shader::setUniform1i(const char *name, int value)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location < 0)
        fprintf(stderr, "Could not transfer uniform %s\n", name);
    else
        glUniform1i(location, value);
    return location >= 0;
}

bool Shader::setUniform1f(const char *name, float value)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location < 0)
        fprintf(stderr, "Could not transfer uniform %s\n", name);
    else
        glUniform1f(location, value);
    return location >= 0;
}

bool Shader::setUniform2fv(const char *name, int count, const float *vector)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location < 0)
        fprintf(stderr, "Could not transfer uniform %s\n", name);
    else
        glUniform2fv(location, count, vector);
    return location >= 0;
}

bool Shader::setUniformMatrix4fv(const char *name, const float *matrix, bool transpose)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location < 0)
        fprintf(stderr, "Could not transfer uniform %s\n", name);
    else
        glUniformMatrix4fv(location, 1, transpose, matrix);
    return location >= 0;
}

SceneGraph::Shader *Shader::createDefault(Node *parent)
{
    return new SceneGraph::Shader(default_vertex_shader,
                                  default_fragment_shader,
                                  DefaultUniforms,
                                  DefaultAttributes,
                                  parent);
}

// Texture2D

Texture2D::Texture2D(GLuint width, GLuint height, GLuint format, const GLvoid *bits, GLuint unit, Node *parent)
    : Node(parent), m_id(0), m_unit(0), m_owner(false)
{
    initialize(width, height, format, bits, unit);
}

Texture2D::Texture2D(Texture2D *other, Node *parent)
    : Node(parent),
      m_id(other ? other->m_id : 0),
      m_unit(other ? other->m_unit : 0),
      m_owner(false)
{
}

Texture2D::~Texture2D()
{
    if (m_owner)
        glDeleteTextures(1, &m_id);
}

bool Texture2D::initialize(GLuint width, GLuint height, GLuint format, const GLvoid *bits, GLuint unit)
{
    m_owner = true;
    m_unit = unit;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

#if 1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
    /*for (unsigned int i = 0; i < images.size(); ++i) {
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, images.at(i).width, images.at(i).height,
                     0, images.at(i).format, GL_UNSIGNED_BYTE, images.at(i).bits);
    }*/
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, bits);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Texture2D::prepare(State *)
{
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&m_old_id);
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&m_old_unit);
}

void Texture2D::execute(State *)
{
    glActiveTexture(GL_TEXTURE0 + m_unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::cleanup(State *)
{
    glActiveTexture(m_old_unit);
    glBindTexture(GL_TEXTURE_2D, m_old_id);
}

// Mesh - FIXME: should use array of structs, rather than struct of arrays

const GLint Mesh::positionElementCount = 3;
const GLint Mesh::positionSize = 3 * sizeof(float);

const GLint Mesh::normalElementCount = 3;
const GLint Mesh::normalSize = 3 * sizeof(float);

const GLint Mesh::texuvElementCount = 2;
const GLint Mesh::texuvSize = 2 * sizeof(float);

Mesh::Mesh(GLenum mode,
           const float *positions, unsigned int positions_size,
           //const float *normals, unsigned int normals_size,
           const float *texuvs, unsigned int texuvs_size,
           const unsigned int *triangles, unsigned int triangles_size,
           Node *parent)
   : Node(parent), m_mode(0), m_elementCount(0), m_owner(false)
{
    initialize(mode,
               positions, positions_size,
               //normals, normals_size,
               texuvs, texuvs_size,
               triangles, triangles_size);
}

Mesh::Mesh(Mesh *other, Node *parent)
    : Node(parent),
      m_mode(other ? other->m_mode : 0),
      m_elementCount(other ? other->m_elementCount : 0),
      m_owner(false)
{
    if (other) {
        m_ids[0] = other->m_ids[0];
        m_ids[1] = other->m_ids[1];
        m_ids[2] = other->m_ids[2];
    }
}

Mesh::~Mesh()
{
    if (m_owner)
        glDeleteBuffers(BufferCount, m_ids);
}

bool Mesh::initialize(GLenum mode,
                      const float *positions, unsigned int positions_size,
                      //const float *normals, unsigned int normals_size,
                      const float *texuvs, unsigned int texuvs_size,
                      const unsigned int *triangles, unsigned int triangles_size)
{
    m_owner = true;
    m_mode = mode;
    m_elementCount = ((triangles_size) / sizeof(unsigned int));

    glGenBuffers(BufferCount, m_ids);

    // positions
    glBindBuffer(GL_ARRAY_BUFFER, m_ids[PositionBuffer]);
    glBufferData(GL_ARRAY_BUFFER, positions_size, positions, GL_STATIC_DRAW);

    // normals
    //glBindBuffer(GL_ARRAY_BUFFER, m_ids[NormalBuffer]);
    //glBufferData(GL_ARRAY_BUFFER, normals_size, normals, GL_STATIC_DRAW);

    // texuvs
    glBindBuffer(GL_ARRAY_BUFFER, m_ids[TexuvBuffer]);
    glBufferData(GL_ARRAY_BUFFER, texuvs_size, texuvs, GL_STATIC_DRAW);

    // triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ids[TriangleBuffer]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles_size, triangles, GL_STATIC_DRAW);

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void Mesh::execute(State *)
{
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);

    // positions
    GLint position_attrib = glGetAttribLocation(program, Shader::position_attribute_name);
    glEnableVertexAttribArray(position_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, m_ids[PositionBuffer]);
    glVertexAttribPointer(position_attrib, positionElementCount, GL_FLOAT, GL_FALSE, positionSize, 0);

    // normals
    //GLint normal_attrib = glGetAttribLocation(program, Shader::normal_attribute_name); // FIXME: crashes on Android
    //glEnableVertexAttribArray(normal_attrib);
    //glBindBuffer(GL_ARRAY_BUFFER, m_ids[NormalBuffer]);
    //glVertexAttribPointer(normal_attrib, normalElementCount, GL_FLOAT, GL_FALSE, normalSize, 0);

    // texuvs
    GLint texuv_attrib = glGetAttribLocation(program, Shader::texuv_attribute_name);
    glEnableVertexAttribArray(texuv_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, m_ids[TexuvBuffer]);
    glVertexAttribPointer(texuv_attrib, texuvElementCount, GL_FLOAT, GL_FALSE, texuvSize, 0);

    // triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ids[TriangleBuffer]);

    // draw
    glDrawElements(m_mode, /*m_vertexCount*/m_elementCount, GL_UNSIGNED_INT, 0);

    // disable
    glDisableVertexAttribArray(position_attrib);
    //glDisableVertexAttribArray(normal_attrib);
    glDisableVertexAttribArray(texuv_attrib);

    // unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
