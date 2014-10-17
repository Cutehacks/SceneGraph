#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <QtOpenGL>

class Rasterizer
{
public:
    static inline void makeCurrent(QOpenGLFunctions *functions) { m_current = functions; } // FIXME

    static inline void glDeleteProgram(GLuint program) { m_current->glDeleteProgram(program); }
    static inline void glDeleteShader(GLuint shader) { m_current->glDeleteShader(shader); }
    static inline void glShaderSource(GLuint shader, GLsizei count, const GLchar ** const string, const GLint *length) { m_current->glShaderSource(shader, count, string, length); }
    static inline void glCompileShader(GLuint shader) { m_current->glCompileShader(shader); }
    static inline void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) { m_current->glGetShaderiv(shader, pname, params); }
    static inline void glGetShaderInfoLog(GLuint shader, GLsizei buffSize, GLsizei *length, GLchar *infoLog) { m_current->glGetShaderInfoLog(shader, buffSize, length, infoLog); }
    static inline GLuint glCreateShader(GLenum type) { return m_current->glCreateShader(type); }
    static inline GLuint glCreateProgram() { return m_current->glCreateProgram(); }
    static inline void glAttachShader(GLuint program, GLuint shader) { m_current->glAttachShader(program, shader); }
    static inline void glLinkProgram(GLuint program) { m_current->glLinkProgram(program); }
    static inline void glGetProgramiv(uint shader, GLenum pname, GLint *params) { m_current->glGetProgramiv(shader, pname, params); }
    static inline void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) { m_current->glGetProgramInfoLog(program, bufSize, length, infoLog); }
    static inline void glValidateProgram(GLuint program) { m_current->glValidateProgram(program); }
    static inline GLint glGetAttribLocation(GLuint program, const GLchar *name) { return m_current->glGetAttribLocation(program, name); }
    static inline void glGetIntegerv(GLenum pname, GLint *params) { m_current->glGetIntegerv(pname, params); }
    static inline void glUseProgram(GLuint program) { m_current->glUseProgram(program); }
    static inline GLint glGetUniformLocation(GLuint program, const GLchar *name) { return m_current->glGetUniformLocation(program, name); }
    static inline void glUniform1i(GLint location, GLint v0) { m_current->glUniform1i(location, v0); }
    static inline void glUniform1f(GLint location, GLfloat v0) { m_current->glUniform1f(location, v0); }
    static inline void glUniform2fv(GLint location, GLsizei count, const GLfloat *value) { m_current->glUniform2fv(location, count, value); }
    static inline void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { m_current->glUniformMatrix4fv(location, count, transpose, value); }
    static inline void glDeleteTextures(GLsizei n, const GLuint *textures) { m_current->glDeleteTextures(n, textures); }
    static inline void glGenTextures(GLsizei n, GLuint *textures) { m_current->glGenTextures(n, textures); }
    static inline void glTexParameteri(GLenum target, GLenum pname, GLint param) { m_current->glTexParameteri(target, pname, param); }
    static inline void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) { m_current->glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels); }
    static inline void glBindTexture(GLenum target, GLuint texture) { m_current->glBindTexture(target, texture); }
    static inline void glActiveTexture(GLenum texture) { m_current->glActiveTexture(texture); }
    static inline void glDeleteBuffers(GLsizei n, const GLuint *buffers) { m_current->glDeleteBuffers(n, buffers); }
    static inline void glGenBuffers(GLsizei n, GLuint *buffers) { m_current->glGenBuffers(n, buffers); }
    static inline void glBindBuffer(GLenum target, GLuint buffer) { m_current->glBindBuffer(target, buffer); }
    static inline void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) { m_current->glBufferData(target, size, data, usage); }
    static inline void glEnableVertexAttribArray(GLuint index) { m_current->glEnableVertexAttribArray(index); }
    static inline void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) { m_current->glVertexAttribPointer(index, size, type, normalized, stride, pointer); }
    static inline void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) { m_current->glDrawElements(mode, count, type, indices); }
    static inline void glDisableVertexAttribArray(GLuint index) { m_current->glDisableVertexAttribArray(index); }
    static inline void glBlendFunc(GLenum sfactor, GLenum dfactor) { m_current->glBlendFunc(sfactor, dfactor); }

private:
    static QOpenGLFunctions *m_current;
};

#endif//RASTERIZER_H
