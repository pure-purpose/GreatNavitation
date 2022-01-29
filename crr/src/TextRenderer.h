#pragma once
#ifndef PROJECT_TEXTRENDERER_H
#define PROJECT_TEXTRENDERER_H


#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include <map>
#include <string>

#include <Shader.h>

class TextRenderer
{
public:
    TextRenderer();
    ~TextRenderer() = default;

    void renderText(Shader &s, std::string text,
                    GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    glm::mat4 projection;
private:
    struct Character {
        GLuint     TextureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        GLuint     Advance;
    };

    std::map<GLchar, Character> Characters;

    unsigned int VAO, VBO;
};


#endif
