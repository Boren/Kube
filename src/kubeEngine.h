#pragma once

#include "chunks/chunkManager.h"
#include "input/inputManager.h"
#include "kubeWindow.h"
#include "renderer/renderer.h"
#include "renderer/scenemanager.h"
#include "renderer/textManager.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

//! Main class keeping track of the whole engine
class Engine {
public:
  Engine();

  bool initialize(std::string windowName);
  void initializeChunkManager(int numX, int numY, int numZ);
  void setSceneManager(SceneManager *sceneManager);
  SceneManager *getSceneManager();
  InputManager *getInputManager();
  Renderer *getRenderer();
  void setCamera(Camera *camera);
  Window *getWindow();

  bool update(float deltaTime);

  float getDeltaTime();

  // Input
  // void handleInput(float deltaTime);
  // void keyPressed(int key, int scancode, int mods);
  // void keyReleased(int key, int scancode, int mods);

private:
  static Engine *instance;

  Window *pWindow;
  SceneManager *pSceneManager;
  InputManager *pInputManager;
  TextManager *pTextManager;
  Renderer *pRenderer;
  Camera *pCamera;

  float m_previousTime = 0.0f;
};
