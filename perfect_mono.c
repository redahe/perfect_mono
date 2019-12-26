#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

static int window_height = 600;
static int window_width = 600;

static struct timeval last_time;

typedef struct {
    GLuint texture_id;
    GLuint size_x;  
    GLuint size_y;
    GLuint bearing_x;
    GLuint bearing_y;
    GLuint advance;
} Character;

typedef struct {
    Character character;
    bool initialized;
} CharEntry;

#define CHAR_SET_SIZE 140000

CharEntry chars[CHAR_SET_SIZE];

FT_Library ft_handle;
FT_Face ft_face;

void init_freetype() {
    if (FT_Init_FreeType(&ft_handle)) {
        fprintf(stderr, "FreeType failed to init");
        exit(-1);
    }

    if (FT_New_Face(ft_handle, "fonts/LDFComicSans.ttf", 0, &ft_face)) {
        fprintf(stderr, "Freetype failed to load font");
        exit(-1);
    }
  
    FT_Set_Pixel_Sizes(ft_face, 0, 32);   
    if (FT_Load_Char(ft_face, 'A', FT_LOAD_RENDER)) {
        fprintf(stderr, "Freetype failed to load glyph");
        exit(-1);
    }
}



GLuint load_char(GLuint index) {
    if (index > CHAR_SET_SIZE) {
        fprintf(stderr, "Char index is too big");
        exit(-1);
    }

    if (chars[index].initialized) {
        return chars[index].character.texture_id; 
    }

    // Load character glyph 
    if (FT_Load_Char(ft_face, index, FT_LOAD_RENDER)) {
        fprintf(stderr, "Freetype failed to load glyph");
        exit(-1);
    }
    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        ft_face->glyph->bitmap.width,
        ft_face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        ft_face->glyph->bitmap.buffer
    );
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Now store character for later use
    Character character = {
        texture, 
        ft_face->glyph->bitmap.width,
        ft_face->glyph->bitmap.rows,
        ft_face->glyph->bitmap_left,
        ft_face->glyph->bitmap_top,
        ft_face->glyph->advance.x
    };
    chars[index] = (CharEntry) {
        character,
        true
   };
   return texture;
}


void draw(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.5f, -0.3f);
    glVertex2f(0.5f, 0.3f);
    glEnd();
    glutSwapBuffers();
}

void reshape(GLint width, GLint height) {
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
    glLoadIdentity();
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

    init_freetype();

    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(handle_keys);
    glutIdleFunc(idle);
    gettimeofday(&last_time, NULL);

    glutMainLoop();
  
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_handle);

    return 0;
}
