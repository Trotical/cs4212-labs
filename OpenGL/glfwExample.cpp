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
#include "camera.h"

struct ShaderBundle {
    sivelab::GLSLObject program;
    GLint projMatrixID   = -1;
    GLint viewMatrixID   = -1;
    GLint modelMatrixID  = -1;
    GLint normalMatrixID = -1;
    GLint lightPosID     = -1;
    GLint diffuseCompID  = -1;
    GLint specularCompID = -1;
    GLint phongExpID     = -1;
    GLint camPosID       = -1;

    void init(const char* vertPath, const char* fragPath, bool useDiffuse, bool useBlinnPhong) {
        program.addShader(vertPath, sivelab::GLSLObject::VERTEX_SHADER);
        program.addShader(fragPath, sivelab::GLSLObject::FRAGMENT_SHADER);
        program.createProgram();

        projMatrixID   = program.createUniform("projMatrix");
        viewMatrixID   = program.createUniform("viewMatrix");
        modelMatrixID  = program.createUniform("modelMatrix");
        normalMatrixID = program.createUniform("normalMatrix");
        lightPosID     = program.createUniform("lightPosWorld");

        if (useDiffuse || useBlinnPhong) {
            diffuseCompID = program.createUniform("diffuseComponent");
        }
        if (useBlinnPhong) {
            specularCompID = program.createUniform("specularComponent");
            phongExpID     = program.createUniform("phongExp");
            camPosID       = program.createUniform("camPosWorld");
        }
    }
};

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
    glClearColor(0.9f, 0.8f, 0.5f, 1.0f);

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    float halfWidth = 15.0f / 2.0f;
    float halfHeight = halfWidth / aspectRatio;
    float nearPlane = 5.0f;
    float farPlane = -5.0f;
    glm::mat4 M_ortho = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);

    PerspectiveCamera cam;
    cam.origin = glm::vec3(0.0f, 0.0f, 8.0f);
    cam.viewDir = glm::vec3(0.0f, 0.0f, -1.0f);
    cam.upDir = glm::vec3(0.0f, 1.0f, 0.0f);
    bool usePerspective = true;

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;

    // VBO with positions and normals (6 floats per vertex)
    GLuint m_triangleVBO[1];
    glGenBuffers(1, m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);

    std::vector<float> host_VertexBuffer{
        // Position (x, y, z)      // Normal (nx, ny, nz)
        -3.0f, -3.0f,  0.0f,        0.0f, 0.0f, 1.0f,  // V0
         3.0f, -3.0f,  0.0f,        0.0f, 0.0f, 1.0f,  // V1
         0.0f,  3.0f,  0.0f,        0.0f, 0.0f, 1.0f   // V2
    };

    GLsizeiptr numBytes = static_cast<GLsizeiptr>(host_VertexBuffer.size() * sizeof(float));
    glBufferData(GL_ARRAY_BUFFER, numBytes, host_VertexBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    host_VertexBuffer.clear();

    // VAO: location 0 = position, location 1 = normal
    GLuint m_VAO;
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Load all 3 shader modes upfront
    const char* vertPath = "vertexShader_PrepForPerFragment.glsl";
    ShaderBundle lambertShader, blinnPhongShader, normalShader;
    lambertShader.init(vertPath, "fragmentShader_lambertian.glsl", true, false);
    blinnPhongShader.init(vertPath, "fragmentShader_blinnPhong.glsl", true, true);
    normalShader.init(vertPath, "fragmentShader_normal.glsl", false, false);

    int shaderMode = 1; // 1 = Lambertian, 2 = Blinn-Phong, 3 = Normal
    std::cout << "Shader mode: 1 = Lambertian, 2 = Blinn-Phong, 3 = Normal" << std::endl;

    glm::vec4 lightPosWorld(2.0f, 4.0f, 8.0f, 1.0f);
    glm::vec3 diffuseColor(0.2f, 0.6f, 1.0f);
    glm::vec3 specularColor(1.0f, 1.0f, 1.0f);
    float phongExp = 32.0f;

    double timeDiff = 0.0, startFrameTime = 0.0, endFrameTime = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        endFrameTime = glfwGetTime();
        timeDiff = endFrameTime - startFrameTime;
        startFrameTime = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float angle = static_cast<float>(glfwGetTime()) * 0.5f;
        glm::mat4 M_model(1.0f);
        M_model = glm::rotate(M_model, angle * 0.25f, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 normalMatrix = glm::transpose(glm::inverse(M_model));
        glm::mat4 M_view = cam.getViewMatrix();
        glm::mat4 M_proj = usePerspective
            ? cam.getPerspectiveMatrix(aspectRatio, 0.1f, 100.0f)
            : M_ortho;

        ShaderBundle* active = &lambertShader;
        if (shaderMode == 2) active = &blinnPhongShader;
        else if (shaderMode == 3) active = &normalShader;

        active->program.activate();
        glUniformMatrix4fv(active->projMatrixID, 1, GL_FALSE, glm::value_ptr(M_proj));
        glUniformMatrix4fv(active->viewMatrixID, 1, GL_FALSE, glm::value_ptr(M_view));
        glUniformMatrix4fv(active->modelMatrixID, 1, GL_FALSE, glm::value_ptr(M_model));
        glUniformMatrix4fv(active->normalMatrixID, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform4fv(active->lightPosID, 1, glm::value_ptr(lightPosWorld));

        if (shaderMode == 1 || shaderMode == 2) {
            glUniform3fv(active->diffuseCompID, 1, glm::value_ptr(diffuseColor));
        }
        if (shaderMode == 2) {
            glUniform3fv(active->specularCompID, 1, glm::value_ptr(specularColor));
            glUniform1f(active->phongExpID, phongExp);
            glUniform3fv(active->camPosID, 1, glm::value_ptr(cam.origin));
        }

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        active->program.deactivate();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && shaderMode != 1) {
            shaderMode = 1;
            std::cout << "Switched to Lambertian shader" << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && shaderMode != 2) {
            shaderMode = 2;
            std::cout << "Switched to Blinn-Phong shader" << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && shaderMode != 3) {
            shaderMode = 3;
            std::cout << "Switched to Normal shader" << std::endl;
        }

        float moveRatePerFrame = 0.05f;
        glm::vec3 fwd = glm::normalize(cam.viewDir);
        glm::vec3 right = glm::normalize(glm::cross(fwd, cam.upDir));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cam.origin += fwd * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cam.origin -= fwd * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cam.origin -= right * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cam.origin += right * moveRatePerFrame;
        }

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