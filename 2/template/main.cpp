#include "../../utils/utils.h"
#include <time.h>
#include "Camera.h"
const unsigned int INTERVAL = 17;// 60 per sec
const unsigned int WIDTH    = 800;
const unsigned int HEIGHT   = 600;

#define STUPID
void warp_pointer(bool freeze) {
    if (!freeze) {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        return;
    }
    glutSetCursor(GLUT_CURSOR_NONE);
#if(defined(WIN32) && defined(STUPID))
    SetCursorPos(
        glutGet(GLUT_INIT_WINDOW_X) + glutGet(GLUT_WINDOW_WIDTH) / 2, 
        glutGet(GLUT_INIT_WINDOW_X) + glutGet(GLUT_WINDOW_HEIGHT) / 2
    );
    return;
#endif//
    glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);//glutWarpPointer(WIDTH, HEIGHT);
}

//dirty hack
#define CLEAR_COLOR .1f, .1f, .1f, 1.0f 


shader_t *main_shader;

model_t *triangle;

Camera g_camera;
bool g_key[256];
bool g_shift_down = false;
bool g_fps_mode = false;
int g_viewport_width = 0;
int g_viewport_height = 0;
bool g_mouse_left_down = false;
bool g_mouse_right_down = false;

// Movement settings
const float g_translation_speed = 0.05;
const float g_rotation_speed = M_PI/180.0*0.2;
clock_t prev = 0;


void display() {

    if (prev != 0 && INTERVAL > 1000.0f*(clock() - prev)/CLOCKS_PER_SEC) {
        return;
    }

    prev = clock();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader

	// 1rst attribute buffer : vertices
    main_shader->set_view_matrix(g_camera.get_matrix());
    glm::mat4 model(1.0f);//glm::rotate(main_shader->view, 60.0f*clock()/CLOCKS_PER_SEC, glm::vec3(0.0, 1.0, 0.0));//60 degrees per sec


    triangle->draw(main_shader->view, GL_TRIANGLES, triangle->count);
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

void Keyboard(unsigned char key, int x, int y)
{
    if(key == 27) {
        exit(0);
    }

    if(key == ' ') {
        g_fps_mode = !g_fps_mode;

        if(g_fps_mode) {
            glutSetCursor(GLUT_CURSOR_NONE);
            glutWarpPointer(WIDTH/2, HEIGHT/2);
        }
        else {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            printf("%f %f\n", g_camera.m_pitch, g_camera.m_yaw);
        }
    }

    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
        g_shift_down = true;
    }
    else {
        g_shift_down = false;
    }

    g_key[key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
    g_key[key] = false;
}

void Timer(int value)
{
    if(g_fps_mode) {
        if(g_key['w'] || g_key['W']) {
            g_camera.Move(g_translation_speed);
        }
        else if(g_key['s'] || g_key['S']) {
            g_camera.Move(-g_translation_speed);
        }
        else if(g_key['a'] || g_key['A']) {
            g_camera.Strafe(g_translation_speed);
        }
        else if(g_key['d'] || g_key['D']) {
            g_camera.Strafe(-g_translation_speed);
        }
        else if(g_mouse_left_down) {
            g_camera.Fly(-g_translation_speed);
        }
        else if(g_mouse_right_down) {
            g_camera.Fly(g_translation_speed);
        }
    }

    glutTimerFunc(1, Timer, 0);
}


void Mouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN) {
        if(button == GLUT_LEFT_BUTTON) {
            g_mouse_left_down = true;
        }
        else if(button == GLUT_RIGHT_BUTTON) {
            g_mouse_right_down = true;
        }
    }
    else if(state == GLUT_UP) {
        if(button == GLUT_LEFT_BUTTON) {
            g_mouse_left_down = false;
        }
        else if(button == GLUT_RIGHT_BUTTON) {
            g_mouse_right_down = false;
        }
    }
}

void MouseMotion(int x, int y)
{
    // This variable is hack to stop glutWarpPointer from triggering an event callback to Mouse(...)
    // This avoids it being called recursively and hanging up the event loop
        //warp_pointer(g_fps_mode);

    static bool just_warped = false;

    if(just_warped) {
        just_warped = false;
        return;
    }

    if(g_fps_mode) {
        int dx = x - WIDTH/2;
        int dy = y - HEIGHT/2;

        if(dx) {
            g_camera.RotateYaw(g_rotation_speed*dx);
        }

        if(dy) {
            g_camera.RotatePitch(-g_rotation_speed*dy);
        }

        glutWarpPointer(WIDTH/2, HEIGHT/2);

        just_warped = true;
    }
}


void update(int value) {
   // Call update() again in 'interval' milliseconds
   glutTimerFunc(INTERVAL, update, 0);
 
   // Redisplay frame
   glutPostRedisplay();
}

void init() {
    glClearColor(CLEAR_COLOR);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, WIDTH, HEIGHT);
    glm::mat4 projection = glm::perspectiveFov(45.0f, (float)WIDTH, (float)HEIGHT, .01f, 300.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    main_shader = new shader_t("shader.vs", "shader.fs");
    main_shader->set_projection_matrix(projection);
    main_shader->set_view_matrix(view);
    g_camera.SetPos(10.0f, 10.0f, 10.0f);
    g_camera.SetPitch(-0.6f);
    g_camera.SetYaw(10.0f);

    triangle = loadOBJ("scene-heavy.obj", main_shader);


}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);

    //glutInitWindowPosition (100, 100);
    glutCreateWindow("2nd task");

    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_3_0)  // check that the machine supports the 2.1 API.
        exit(1); // or handle the error in a nicer way

    init ();

    glutIdleFunc(display);
    glutDisplayFunc(display);
    glutTimerFunc(1, Timer, 0);
    glutMouseFunc(Mouse);
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(MouseMotion);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUp);

    glutMainLoop();

    delete main_shader;
    delete triangle;
    //delete vertex_buffer;
    //delete color_buffer;

    return 0; /* ISO C requires main to return int. */
}