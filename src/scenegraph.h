#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <list>
#include <stack>

#include "rasterizer.h"

namespace SceneGraph {

    class Node;

    class State
    {
    public:
        State();
        ~State();

        // misc
        void reset();
        void execute(Node *node);

        // matrix stack
        void pushMatrix();
        void multiplyMatrix(const float *matrix);
        void popMatrix();
        const float *currentMatrix();

        // projection
        void setOrtographicProjection(float left, float right,
                                      float bottom, float top,
                                      float near = -1.0, float far = 1.0);
        void setOrtographicProjectionEx(float aspect, float fov_y,
                                        float near = -1.0, float far = 1.0);
        void setPerspectiveProjection(float left, float right,
                                      float bottom, float top,
                                      float near = -1.0, float far = 1.0);
        void setPerspectiveProjectionEx(float aspect, float fov_y,
                                        float near = -1.0, float far = 1.0);
        void setProjectionMatrix(const float *matrix);
        const float *projectionMatrix();

    private:
        void clearMatrices();

    private:
        std::stack<float*> m_matrices;
        float m_projection_matrix[16];
    };

    class Node : protected Rasterizer
    {
        friend class State;
    public:
        Node(Node *parent = 0);
        virtual ~Node();

        Node *parent() const;
        std::list<Node*> children() const;

        virtual bool enabled(State *state);
        virtual void prepare(State *state);
        virtual void execute(State *state);
        virtual bool visible(State *state);
        virtual void cleanup(State *state);
        virtual void update(State *state);

   protected:
        Rasterizer *m_rasterizer;

    private:
        Node *m_parent;
        std::list<Node*> m_children;
    };

    class Transformation : public Node
    {
    public:
        Transformation(const float *matrix, Node *parent = 0);
        ~Transformation();

        void prepare(State *state);
        void execute(State *state);
        void cleanup(State *state);

        void scale(float sx, float sy, float sz);
        void translate(float dx, float dy, float dz);
        void rotate(float vx, float vy, float vz, float radians);

    protected:
        void multiply(const float *transformation);

        float *matrix();
        void setMatrix(const float *matrix);

        static void create_rotation_matrix(float vx, float vy, float vz, float radians, float *destination);

    private:
        float m_matrix[16];
    };

    class Shader : public Node
    {
    public:
        enum Uniforms { ProjectionModelViewUniform = 1, TextureSamplerUniform = 2, DefaultUniforms = 3 };
        enum Attributes { PositionAttribute = 1, NormalAttribute = 2, TexuvAttribute = 4, DefaultAttributes = 5 };

        Shader(const char *vertex_source = default_vertex_shader,
               const char *fragment_source = default_fragment_shader,
               unsigned int uniforms = DefaultUniforms,
               unsigned int attributes = DefaultAttributes,
               Node *parent = 0);
        Shader(Shader *other, Node *parent = 0);
        ~Shader();

        void prepare(State *state);
        void execute(State *state);
        void cleanup(State *state);

        bool setUniform1i(const char *name, int value);
        bool setUniform1f(const char *name, float value);
        bool setUniform2fv(const char *name, int count, const float *vector);
        bool setUniformMatrix4fv(const char *name, const float *matrix, bool transpose = false);

        static const char *default_vertex_shader;
        static const char *default_fragment_shader;
        static const char *projection_model_view_matrix_uniform_name;
        static const char *texture_sampler_uniform_name;
        static const char *position_attribute_name;
        static const char *normal_attribute_name;
        static const char *texuv_attribute_name;

        static Shader *createDefault(Node *parent = 0); // FIXME

    protected:
        bool initialize(const char *vertex_source,
                        const char *fragment_source,
                        unsigned int uniforms,
                        unsigned int attributes);

    private:
        GLuint m_program;
        GLuint m_old_program;
        unsigned int m_uniforms;
        bool m_owner;
    };

    class Texture2D : public Node
    {
    public:
        Texture2D(GLuint width, GLuint height, GLuint format, const GLvoid *bits, GLuint unit = 0, Node *parent = 0);
        Texture2D(Texture2D *other, Node *parent = 0);
        ~Texture2D();

        void prepare(State *state);
        void execute(State *state);
        void cleanup(State *state);

    protected:
        bool initialize(GLuint width, GLuint height, GLuint format, const GLvoid *bits, GLuint unit = 0);

    private:
        GLuint m_id;
        GLuint m_old_id;
        GLuint m_unit;
        GLuint m_old_unit;
        bool m_owner;
    };

    class Mesh : public Node
    {
    public:
        Mesh(GLenum mode,
             const float *positions, unsigned int positions_size,
             //const float *normals, unsigned int normals_size,
             const float *texuvs, unsigned int texuvs_size,
             const unsigned int *triangles, unsigned int triangles_size,
             Node *parent = 0);
        Mesh(Mesh *other, Node *parent = 0);
        ~Mesh();

        void execute(State *state);

    protected:
        bool initialize(GLenum mode,
                        const float *positions, unsigned int positions_size,
                        //const float *normals, unsigned int normals_size,
                        const float *texuvs, unsigned int texuvs_size,
                        const unsigned int *triangles, unsigned int triangles_size);

    private:
        static const GLint positionElementCount;
        static const GLint positionSize;

        static const GLint normalElementCount;
        static const GLint normalSize;

        static const GLint texuvElementCount;
        static const GLint texuvSize;

        enum Buffer {
            PositionBuffer = 0,
            //NormalBuffer = 1,
            TexuvBuffer,
            TriangleBuffer,
            BufferCount = TriangleBuffer + 1
        };

        GLenum m_mode;
        GLuint m_ids[3];
        GLuint m_elementCount;
        bool m_owner;
    };

}; // SceneGraph

#endif//SCENEGRAPH_H
