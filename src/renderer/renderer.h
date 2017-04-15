#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../chunks/chunkManager.h"
#include "../scene/camera.h"
#include "../scene/light.h"
#include "../scene/scenemanager.h"
#include "shader.h"
#include "textManager.h"

class SceneManager;

//! \enum Render Mode
enum RenderMode {
    RENDERMODE_WIREFRAME = 0, //!< Renders object with edge lines only
    RENDERMODE_SHADED,        //!< Renders object shaded
};

//! Responsible for rendering all scene objects
class Renderer {

public:
    /*!
     * \brief Initialize renderer
     * \param windowWidth Width of window
     * \param windowHeight Height of window
     * \return
     */
    bool initialize(int windowWidth, int windowHeight);

    /*!
     * \brief Renders the current scene using projection from camera
     * \param sceneManager Scenemanager to use in rendering
     * \param camera Camera object to use in rendering
     */
    void render(SceneManager *sceneManager, Camera *camera,
                TextManager *textManager);

    /*!
     * \brief Set a new render mode
     * \param mode RenderMode to set
     */
    void setRenderMode(RenderMode mode);

    RenderMode getCurrentRenderMode();

    float rotateAngle = 0;

    Shader *getDefaultShader();

private:
    int m_windowHeight; //!< Current height of window
    int m_windowWidth;  //!< Current width of window

    Shader m_defaultShader; //!< Shader to use by renderer if no other shader is
    //! selected

    RenderMode m_renderMode = RENDERMODE_SHADED;

    std::vector<double> m_profiling;
    double m_profilingMean = 0;
    double m_lastPrint = 0;
};
