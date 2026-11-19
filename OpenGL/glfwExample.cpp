#include <cstdlib>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSL.h"

int CheckGLErrors(const char *s)
{
    int errCount = 0;
    return errCount;
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit()) {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    int winWidth = 1000;
    float aspectRatio = 1.0f;
    int winHeight = static_cast<int>(winWidth / aspectRatio);

    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "GLFW Example", NULL, NULL);
    if (!window) {
        std::cerr << "GLFW did not create a window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW Error! glewInit failed, exiting." << std::endl;
        exit(EXIT_FAILURE);
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.7f, 0.4f, 0.2f, 1.0f);

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    float halfWidth = 15.0f / 2.0f;
    float halfHeight = halfWidth / aspectRatio;
    glm::mat4 projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -10.0f, 10.0f);

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;

    GLuint m_triangleVBO[1];
    glGenBuffers(1, m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);

    std::vector<float> host_VertexBuffer{
        // Position (x, y, z)      // Color (r, g, b)
        -0.6f, -0.4f,  0.0f,        1.0f, 0.2f, 0.3f,  // V0
         0.6f, -0.4f,  0.0f,        0.2f, 0.9f, 0.4f,  // V1
         0.0f,  0.65f, 0.0f,        0.2f, 0.5f, 1.0f   // V2
    };

    GLsizeiptr numBytes = static_cast<GLsizeiptr>(host_VertexBuffer.size() * sizeof(float));
    glBufferData(GL_ARRAY_BUFFER, numBytes, host_VertexBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    host_VertexBuffer.clear();

    GLuint m_VAO;
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);

    // Location 0 position (3 floats, stride = 6 floats, offset = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

    // Location 1 color (3 floats, stride = 6 floats, offset = 3 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    sivelab::GLSLObject shader;
    shader.addShader("vertexShader.glsl", sivelab::GLSLObject::VERTEX_SHADER);
    shader.addShader("fragmentShader.glsl", sivelab::GLSLObject::FRAGMENT_SHADER);
    shader.createProgram();

    double timeDiff = 0.0, startFrameTime = 0.0, endFrameTime = 0.0;

    
    while (!glfwWindowShouldClose(window))
    {
        endFrameTime = glfwGetTime();
        timeDiff = endFrameTime - startFrameTime;
        startFrameTime = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Render objects here */
        shader.activate();
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        shader.deactivate();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            std::cout << "fps: " << 1.0 / timeDiff << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glfwTerminate();
    return 0;
}