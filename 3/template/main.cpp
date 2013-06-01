#include "../../utils/utils.h"
#include <time.h>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif//_USE_MATH_DEFINES
#include <math.h>
const unsigned int INTERVAL = 17;// 60 per sec
const unsigned int WIDTH    = 800;
const unsigned int HEIGHT   = 600;

//dirty hack
#define CLEAR_COLOR .1f, .1f, .1f, 1.0f 


shader_t *main_shader;

model_t *triangle;

GLuint lightDir;

glm::vec3 light_vec = glm::vec3(1.0f);

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader

	// 1rst attribute buffer : vertices
    glm::mat4 model = glm::rotate(main_shader->view, 60.0f*clock()/CLOCKS_PER_SEC, glm::vec3(0.0, 1.0, 1.0));//60 degrees per sec

    glUniform3f(lightDir, light_vec.x, light_vec.y, light_vec.z);

    triangle->draw_indexed(model, GL_TRIANGLES, triangle->count);
/*    main_shader->set_view_matrix(model);

    main_shader->bind();

    vertex_buffer->bind();
    color_buffer->bind();

    // Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);*/

	// Swap buffers
    glutSwapBuffers();
}

void update(int value) {
   // Call update() again in 'interval' milliseconds
   glutTimerFunc(INTERVAL, update, 0);
 
   // Redisplay frame
   glutPostRedisplay();
}

void mouse_motion(int x, int y) {
    light_vec.x = 10.0f*(-0.5f + (float)x/WIDTH);
    light_vec.y = -10.0f*(-0.5f + (float)y/HEIGHT);
    light_vec.z = 1.0f;
}


void init() {
    glClearColor(CLEAR_COLOR);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, WIDTH, HEIGHT);
    glm::mat4 projection = glm::perspectiveFov(45.0f, (float)WIDTH, (float)HEIGHT, .01f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    main_shader = new shader_t("shader.vs", "shader.fs");
    main_shader->set_projection_matrix(projection);
    main_shader->set_view_matrix(view);

    lightDir = glGetUniformLocation(main_shader->program, "lightDir");

    unsigned int sphere_vertical_segs = 30;
    unsigned int sphere_radial_segs = 30;
    
    float sphere_radius = 1.0f;
    float r = sphere_radius;

    std::vector<float> tmp_mesh;
    std::vector<float> tmp_norm;
    std::vector<float> tmp_colr;

    std::vector<unsigned int> tmp_indx;

    // sphere in radial coordinates
    // may be optimized even more
    for (int i = 0; i < sphere_vertical_segs; ++i) {
        float y1 = cos(M_PI*i/sphere_vertical_segs);
        float y2 = cos(M_PI*(i + 1)/sphere_vertical_segs);
        float _y1 = sin(M_PI*i/sphere_vertical_segs);
        float _y2 = sin(M_PI*(i + 1)/sphere_vertical_segs);
        for (int j = 0; j < sphere_radial_segs; ++j) {
            float x1 = cos(2*M_PI*j/sphere_radial_segs);
            float x2 = cos(2*M_PI*(j + 1)/sphere_radial_segs);
            float z1 = sin(2*M_PI*j/sphere_radial_segs);
            float z2 = sin(2*M_PI*(j + 1)/sphere_radial_segs);
            unsigned int index_offset = (i*sphere_radial_segs + j)*4;

            tmp_mesh.push_back(r*_y1*x1); tmp_mesh.push_back(r*y1); tmp_mesh.push_back(r*_y1*z1);
            tmp_norm.push_back(_y1*x1);   tmp_norm.push_back(y1);   tmp_norm.push_back(_y1*z1);

            tmp_mesh.push_back(r*_y1*x2); tmp_mesh.push_back(r*y1); tmp_mesh.push_back(r*_y1*z2);
            tmp_norm.push_back(_y1*x2);   tmp_norm.push_back(y1);   tmp_norm.push_back(_y1*z2);

            tmp_mesh.push_back(r*_y2*x1); tmp_mesh.push_back(r*y2); tmp_mesh.push_back(r*_y2*z1);
            tmp_norm.push_back(_y2*x1);   tmp_norm.push_back(y2);   tmp_norm.push_back(_y2*z1);

            tmp_mesh.push_back(r*_y2*x2); tmp_mesh.push_back(r*y2); tmp_mesh.push_back(r*_y2*z2);
            tmp_norm.push_back(_y2*x2);   tmp_norm.push_back(y2);   tmp_norm.push_back(_y2*z2);

            tmp_indx.push_back(index_offset + 0);
            tmp_indx.push_back(index_offset + 1);
            tmp_indx.push_back(index_offset + 3);

            tmp_indx.push_back(index_offset + 0);
            tmp_indx.push_back(index_offset + 2);
            tmp_indx.push_back(index_offset + 3);
        }
    }

    GLfloat *mesh = new GLfloat[tmp_mesh.size()];
    GLfloat *norm = new GLfloat[tmp_norm.size()];

    GLushort *indx = new GLushort[tmp_norm.size()];

    for (int i = 0; i < tmp_mesh.size(); ++i) {
        mesh[i] = tmp_mesh[i];
    }
    for (int i = 0; i < tmp_norm.size(); ++i) {
        norm[i] = tmp_norm[i];
    }
    for (int i = 0; i < tmp_indx.size(); ++i) {
        indx[i] = tmp_indx[i];
    }

    buffer_t *vertex_buffer = new buffer_t(0, mesh, tmp_mesh.size());
    buffer_t *norm_buffer = new buffer_t(1, norm, tmp_norm.size());
    buffer_t *indx_buffer = new buffer_t(indx, tmp_indx.size());
    delete[] mesh;
    delete[] norm;
    delete[] indx;

    triangle = new model_t(main_shader);

    triangle->buffers.push_back(indx_buffer);
    triangle->buffers.push_back(vertex_buffer);
    triangle->buffers.push_back(norm_buffer);
    triangle->count = tmp_indx.size();


}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);

    //glutInitWindowPosition (100, 100);
    glutCreateWindow("3rd task");

    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_3_0)  // check that the machine supports the 2.1 API.
        exit(1); // or handle the error in a nicer way

    init ();

    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouse_motion);
    glutMotionFunc(mouse_motion);
    glutTimerFunc(INTERVAL, update, 0);
    glutMainLoop();

    delete main_shader;
    delete triangle;
    //delete vertex_buffer;
    //delete color_buffer;

    return 0; /* ISO C requires main to return int. */
}