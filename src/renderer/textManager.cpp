#include "textManager.h"

#include "../utils/log.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>

GLuint VAO, VBO;

struct Character {
  GLuint TextureID;   // ID handle of the glyph texture
  glm::ivec2 Size;    // Size of glyph
  glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
  GLuint Advance;     // Offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

TextManager::TextManager() {
  if (FT_Init_FreeType(&ft)) {
    Log::message(Log::FATAL, "Text Manager", "Failed to initialize freetype");
  }

  if (FT_New_Face(ft, "assets/fonts/Roboto-Regular.ttf", 0, &face)) {
    Log::message(Log::ERROR, "Text Manager", "Could not load font.");
  }

  FT_Set_Pixel_Sizes(face, 0, 48);

  if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
    Log::message(Log::FATAL, "Text Manager", "Could not load character.");
  }

  Log::message(Log::DEBUG, "Text Manager", "Loaded font");

    m_textShader.compileShader("assets/shaders/BasicText.vert", Shader::VERTEX);
    m_textShader.compileShader("assets/shaders/BasicText.frag", Shader::FRAGMENT);
  m_textShader.link();
  m_textShader.use();

  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(1280), 0.0f,
                                    static_cast<GLfloat>(720));
  glUniformMatrix4fv(glGetUniformLocation(m_textShader.getHandle(), "projection"), 1,
                     GL_FALSE, glm::value_ptr(projection));

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load first 128 characters of ASCII set
  for (GLubyte c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
      continue;
    }
    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Now store character for later use
    Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint) face->glyph->advance.x};
    Characters.insert(std::pair<GLchar, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  // Configure VAO/VBO for texture quads
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void TextManager::renderText(std::string text, GLfloat x, GLfloat y,
                             GLfloat scale, glm::vec3 color) {

  m_textShader.use();
  glUniform3f(glGetUniformLocation(m_textShader.getHandle(), "textColor"),
              color.x, color.y, color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // Iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = Characters[*c];

    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    // Update VBO for each character
    GLfloat vertices[6][4] = {
        {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
        {xpos + w, ypos, 1.0, 1.0},

        {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
        {xpos + w, ypos + h, 1.0, 0.0}};
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, sizeof(vertices),
        vertices); // Be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Now advance cursors for next glyph (note that advance is number of 1/64
    // pixels)
    x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6
                                    // = 64 (divide amount of 1/64th pixels by
                                    // 64 to get amount of pixels))
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
