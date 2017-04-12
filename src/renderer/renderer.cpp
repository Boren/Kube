// Include standard headers
#include <fstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>

// Include GLFW (Window Manager)
#include <GLFW/glfw3.h>

// Include GLM (OpenGL Mathematics)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

#include "../chunks/chunkManager.h"
#include "../utils/log.h"
#include "renderer.h"
#include "shader.h"

bool Renderer::initialize(int windowWidth, int windowHeight) {
  m_windowHeight = windowHeight;
  m_windowWidth = windowWidth;

  // Set background color
  glClearColor(135 / 255.0f, 206 / 255.0f, 255 / 255.0f, 1);

  // Enable depth test
  // glEnable(GL_DEPTH_TEST);

  // Accept fragment if it closer to the camera than the former one
  // glDepthFunc(GL_LESS);

  // Cull back face to optimize rendering
  glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);

  // Load default shader
  m_defaultShader.compileShader("assets/shaders/BasicPhong.vs", Shader::VERTEX);
  m_defaultShader.compileShader("assets/shaders/BasicPhong.fs",
                                Shader::FRAGMENT);
  m_defaultShader.link();
  m_defaultShader.use();

  m_defaultShader.setUniform1i("ambientOcclusionEnabled", 1);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  return true;
}

void Renderer::render(SceneManager *sceneManager, Camera *camera,
                      TextManager *textManager) {

  double renderStartTime = glfwGetTime();
  int numVertices = 0;

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(8, 8, 8));
  model = glm::rotate(model, rotateAngle, glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-8, -8, -8));
  glm::mat4 viewProjection =
      camera->getProjectionMatrix() * camera->getViewMatrix();
  glm::mat4 MVP =
      camera->getProjectionMatrix() * camera->getViewMatrix() * model;

  m_defaultShader.use();
  m_defaultShader.setUniformMatrix4fv("camera", viewProjection);
  m_defaultShader.setUniformMatrix4fv("model", model);
  m_defaultShader.setUniformMatrix4fv("MVP", MVP);
  m_defaultShader.setUniform3f("cameraPosition", camera->getPosition().x,
                               camera->getPosition().y,
                               camera->getPosition().z);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch (m_renderMode) {
  case RENDERMODE_SHADED:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;

  case RENDERMODE_WIREFRAME:
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  }

  sceneManager->render(this);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  char text[256];
  sprintf(text, "Render: %.3f ms (%.0f FPS)", m_profilingMean,
          1000 / m_profilingMean);
  textManager->renderText(text, 5.0f, 5.0f, 0.4f, glm::vec3(0.2f, 0.2f, 0.2f));

  text[256];
  sprintf(text, "Camera: X:%.0f Y:%.0f Z:%.0f", camera->getPosition().x,
          camera->getPosition().y, camera->getPosition().z);
  textManager->renderText(text, 5.0f, 23.0f, 0.4f, glm::vec3(0.2f, 0.2f, 0.2f));

  text[256];
  sprintf(text, "Vertices: %d",
          sceneManager->getChunkManager()->getNumberOfVertices());
  textManager->renderText(text, 5.0f, 41.0f, 0.4f, glm::vec3(0.2f, 0.2f, 0.2f));

  m_profiling.push_back((glfwGetTime() - renderStartTime) * 1000);

  if (glfwGetTime() > m_lastPrint + 0.3) {
    double sum = std::accumulate(m_profiling.begin(), m_profiling.end(), 0.0);
    m_profilingMean = sum / m_profiling.size();
    m_profiling.clear();
    m_lastPrint = glfwGetTime();
  }
}

void Renderer::setRenderMode(RenderMode mode) { m_renderMode = mode; }

RenderMode Renderer::getCurrentRenderMode() { return m_renderMode; }

Shader *Renderer::getDefaultShader() { return &m_defaultShader; }
