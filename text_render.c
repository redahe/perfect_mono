#include "text_render.h"
#include "utf8.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct {
    GLuint texture_id;
    GLuint size_x;  
    GLuint size_y;
    GLint bearing_x;
    GLint bearing_y;
    GLint advance;
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

    if (FT_New_Face(ft_handle, "fonts/font.ttf", 0, &ft_face)) {
        fprintf(stderr, "Freetype failed to load font");
        exit(-1);
    }
  
    FT_Set_Pixel_Sizes(ft_face, 0, 32);   
    if (FT_Load_Char(ft_face, 'A', FT_LOAD_RENDER)) {
        fprintf(stderr, "Freetype failed to load glyph");
        exit(-1);
    }
}

void close_freetype() {
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_handle);
}

Character load_char(GLuint index) {

    if (index > CHAR_SET_SIZE) {
        fprintf(stderr, "Char index is too big");
        exit(-1);
    }

    if (chars[index].initialized) {
        return chars[index].character; 
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
        GL_LUMINANCE,
        ft_face->glyph->bitmap.width,
        ft_face->glyph->bitmap.rows,
        0,
        GL_LUMINANCE,
        GL_UNSIGNED_BYTE,
        ft_face->glyph->bitmap.buffer
    );
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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
   return character;
}

void draw_text(GLdouble x, GLdouble y, GLfloat scale, char* text) {

    uint32_t code;
    uint32_t decode_state = 0;
    while (*text != '\0') {

        if (decode(&decode_state, &code, (uint8_t)*text) != UTF8_ACCEPT) {
            text++;
            continue;
        }

        Character chr = load_char(code);
        GLdouble cx = x + chr.bearing_x * scale;
        GLdouble cy = y - chr.bearing_y * scale;
        
        GLfloat vertices[6][2] = {
            { cx, cy + chr.size_y * scale },            
            { cx, cy },
            { cx + chr.size_x * scale, cy },

            { cx, cy + chr.size_y * scale},
            { cx + chr.size_x * scale, cy },
            { cx + chr.size_x * scale, cy + chr.size_y * scale}           
        };
       GLfloat tex_coords[6][2] = {
            { 0.0, 1.0 },
            { 0.0, 0.0 },            
            { 1.0, 0.0 },

            { 0.0, 1.0 },
            { 1.0, 0.0 },
            { 1.0, 1.0 },
        };

        glColor3f(0.6f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, chr.texture_id);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(tex_coords[0][0], tex_coords[0][1]);
        glVertex2f(vertices[0][0], vertices[0][1]);
        glTexCoord2f(tex_coords[1][0], tex_coords[1][1]);
        glVertex2f(vertices[1][0], vertices[1][1]);
        glTexCoord2f(tex_coords[2][0], tex_coords[2][1]);
        glVertex2f(vertices[2][0], vertices[2][1]);
        glTexCoord2f(tex_coords[3][0], tex_coords[3][1]);
        glVertex2f(vertices[3][0], vertices[3][1]);
        glTexCoord2f(tex_coords[4][0], tex_coords[4][1]);
        glVertex2f(vertices[4][0], vertices[4][1]);
        glTexCoord2f(tex_coords[5][0], tex_coords[5][1]);
        glVertex2f(vertices[5][0], vertices[5][1]);
        glEnd();
        x += (chr.advance >> 6) * scale; 
        text++;
   } 	
   glBindTexture(GL_TEXTURE_2D, 0);                                      
}
