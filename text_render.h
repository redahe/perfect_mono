#ifndef H_TEXT_RENDER
#define H_TEXT_RENDER

#include <GL/gl.h>

void init_freetype();
void draw_text(GLdouble x, GLdouble y, GLfloat scale, char* text);
void close_freetype();

#endif
