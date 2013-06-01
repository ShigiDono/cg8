#include "utils.h"
#include <time.h>
const unsigned int INTERVAL = 17;// 60 per sec
const unsigned int WIDTH    = 800;
const unsigned int HEIGHT   = 600;

//dirty hack
#define CLEAR_COLOR .1f, .1f, .1f, 1.0f 


shader_t *main_shader;

model_t *triangle;

void display() {

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader

	// 1rst attribute buffer : vertices
    glm::mat4 model = glm::rotate(main_shader->view, 60.0f*clock()/CLOCKS_PER_SEC, glm::vec3(0.0, 1.0, 0.0));//60 degrees per sec


    triangle->draw(model, GL_TRIANGLES, 3);
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

void init() {
    glClearColor(CLEAR_COLOR);
    glViewport(0, 0, WIDTH, HEIGHT);
    glm::mat4 projection = glm::perspectiveFov(45.0f, (float)WIDTH, (float)HEIGHT, .01f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    main_shader = new shader_t("shader.vs", "shader.fs");
    main_shader->set_projection_matrix(projection);
    main_shader->set_view_matrix(view);

    GLfloat mesh[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

    GLfloat color[] = { 
//      r     g     b
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

    buffer_t *vertex_buffer = new buffer_t(0, mesh, 9);
    buffer_t *color_buffer = new buffer_t(1, color, 9);

    triangle = new model_t(main_shader);

    triangle->buffers.push_back(vertex_buffer);
    triangle->buffers.push_back(color_buffer);


}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);

    //glutInitWindowPosition (100, 100);
    glutCreateWindow("template");

    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_3_0)  // check that the machine supports the 2.1 API.
        exit(1); // or handle the error in a nicer way

    init ();

    glutDisplayFunc(display);
    glutTimerFunc(INTERVAL, update, 0);
    glutMainLoop();

    delete main_shader;
    delete triangle;
    //delete vertex_buffer;
    //delete color_buffer;

    return 0; /* ISO C requires main to return int. */
}