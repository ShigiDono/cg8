#ifndef _UTILS_
#define _UTILS_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>

struct shader_t {
    shader_t(const char * vertex_file_path, const char * fragment_file_path);
    void bind();
    
    void set_projection_matrix(const glm::mat4 &matrix);
    void set_view_matrix(const glm::mat4 &matrix);
    
    void apply_model_matrix(const glm::mat4 &matrix);
    void apply_normal_matrix(const glm::mat3 &matrix);

    GLuint program;
    GLuint projection_matrix;
    GLuint model_view_matrix;
    GLuint normal_matrix;
    glm::mat4 projection;
    glm::mat4 view;
};

struct buffer_t {
    buffer_t(GLuint attribute, GLfloat *vertices, unsigned int count, unsigned int size = 3);
    buffer_t(GLushort *indices, unsigned int count, unsigned int size = 3);
    void bind();

    GLuint attribute;
    GLuint buffer;
    GLuint count;
    GLuint size;
};


struct model_t {
    std::vector<buffer_t*> buffers;
    shader_t *shader;
    unsigned int count;

    model_t(shader_t *shader_param);
    ~model_t();

    void draw(glm::mat4 matrix, GLuint primitive_kind, GLuint count);
    void draw_indexed(glm::mat4 matrix, GLuint primitive_kind, GLuint count);
};

model_t *loadOBJ(const char * path, shader_t *shader);



buffer_t create_vertex_array(GLuint attribute, GLfloat *vertices, unsigned int count, unsigned int size = 3);

shader_t load_shader(const char * vertex_file_path,const char * fragment_file_path);

#endif//_UTILS_