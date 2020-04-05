#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "text_render.h"

static int window_height = 600;
static int window_width = 600;

static struct timeval last_time;

void draw(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_text(0.0f, 100.0, 1.0, "Кирилица");
    glutSwapBuffers();
}

void reshape(GLint width, GLint height) {
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, window_height, 0, 0, 1);
}

void idle(void) {
    double delta;
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    delta = (double)(time_now.tv_sec  - last_time.tv_sec) +
    1.0e-6*(time_now.tv_usec - last_time.tv_usec);
    last_time = time_now;
    // Do the animation
    glutPostRedisplay();
}

void handle_keys(unsigned char key, int x, int y) {
    switch (key) {
        case 27:             
            exit (0);
            break;
    }
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitWindowSize(window_width, window_height);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Perfect Mono");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glEnable(GL_TEXTURE_2D);
    init_freetype();

    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(handle_keys);
    glutIdleFunc(idle);
    gettimeofday(&last_time, NULL);

    glutMainLoop();
  
    close_freetype();
    return 0;
}
