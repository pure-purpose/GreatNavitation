//
// Created by 何昊 on 2018/03/05.
//

#include <iostream>
#include <algorithm>

// GLM Math Library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD: A library that wraps OpenGL functions to make things easier
//       Note that GLAD MUST be included before GLFW
#include "glad/glad.h"
// GLFW: A library that helps us manage windows
#include <GLFW/glfw3.h>

// Wrapper classes to make things a little easier
#include <Shader.h>
#include <Camera.h>
#include "Skybox.h"
#include "TextRenderer.h"

// Water related header file
#include "Ocean.h"
#include <Model.h>

// **********GLFW window related functions**********
// Returns pointer to a initialized window with OpenGL context set up
GLFWwindow *init();
// Sometimes user might resize the window. so the OpenGL viewport should be adjusted as well.
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
// User input is handled in this function
void processInput(GLFWwindow *window);
// Mouse input is handled in this function
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
// Render xyz coordinate in world space
//void renderCoordinates(const glm::mat4 &view, const glm::mat4 &proj);


// **********GLFW window related attributes**********
int gScreenWidth = 800;
int gScreenHeight = 600;
float gDeltaTime = 0.0f;
float gLastFrame = 0.0f;

// Global vaiables and flags
Camera gCamera;
bool gDrawNormals = false;
bool gPause = false;

glm::vec3 shipPosition(0.0f, 0.0f, 50.0f);
float shipForward = 0;// glm::pi<float>() / 2;
glm::vec3 shipForwardxz(sin(shipForward), 0, cos(shipForward));
float shipVelocity;
glm::vec3 relative(12, 50, -50);

float wocao[] = { 20.418, 56.466, -69.127 ,0.12259, -0.40354, -0.9067 , 0.0541, 0.915, -0.4 };
int  aaa = 1;
int main()
{
    GLFWwindow *window = init();
    if (window == nullptr) {
        std::cout << "Failed to initialize GLFW and OpenGL!" << std::endl;
        return -1;
    }

    // Load shaders
    Shader shader("./shaders/Water2.vert", "./shaders/Water2.frag");
    Shader textShader("./shaders/TextShader.vert", "./shaders/TextShader.frag");
    Shader normalShader("./shaders/DrawNormal.vert", "./shaders/DrawNormal.frag",
                        "./shaders/DrawNormal.geom");


    Model ship("./model/Boat/boat.obj");
    Shader shipshader("./model/Boat/boat.vs", "./model/Boat/boat.fs");
    // Initialize fonts
    TextRenderer textRenderer;

    // Initialize skybox
    std::vector<std::string> skyboxPaths = {
            "./textures/TropicalSunnyDay/TropicalSunnyDayLeft2048.png",
            "./textures/TropicalSunnyDay/TropicalSunnyDayRight2048.png",
            "./textures/TropicalSunnyDay/TropicalSunnyDayUp2048.png",
            "./textures/TropicalSunnyDay/TropicalSunnyDayDown2048.png",
            "./textures/TropicalSunnyDay/TropicalSunnyDayFront2048.png",
            "./textures/TropicalSunnyDay/TropicalSunnyDayBack2048.png",
    };
    std::vector<std::string> skyboxPaths2 = {
            "./textures/SunSet/SunSetLeft2048.png",
            "./textures/SunSet/SunSetRight2048.png",
            "./textures/SunSet/SunSetUp2048.png",
            "./textures/SunSet/SunSetDown2048.png",
            "./textures/SunSet/SunSetFront2048.png",
            "./textures/SunSet/SunSetBack2048.png",
    };
    Shader skyboxShader("./shaders/SkyboxShader.vert", "./shaders/SkyboxShader.frag");
    Skybox skybox(skyboxPaths);

    // Necessary OpenGL Parameters
    glEnable(GL_DEPTH_TEST);
    // Enable gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);
    // Enable anti-aliasing
    glEnable(GL_MULTISAMPLE);
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //gCamera.Position = { 35.9971, 141.684, 76.8194 };
    gCamera.Position = shipPosition + relative;//glm::vec3(0.0f, 50.0f, 0.0f);///////////////////////
    gCamera.MovementSpeed = 5.0f;
    /*gCamera.target = shipPosition;
    gCamera.relative = relative;
    gCamera.Front = glm::normalize(-relative);*/
    //gCamera.updateCameraVectors();

    Ocean ocean(glm::vec2(0.2f, 2.0f), 64, 0.05f);
    ocean.generateWave((float)glfwGetTime());
    glm::vec3 deepWaterColorSunset = glm::vec3(powf(0.14f, 2.2f),
                                         powf(0.15f, 2.2f),
                                         powf(0.16f, 2.2f));
    glm::vec3 deepWaterColorSunny = glm::vec3(powf(0.11f, 2.2f),
                                         powf(0.18f, 2.2f),
                                         powf(0.35f, 2.2f));
    // Pass the vertex data to GPU
    unsigned int VBO, EBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.vertexCount, ocean.vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ocean.indexCount, ocean.indices, GL_STATIC_DRAW);

    short fpscount = 5;
    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate how much time since last frame
        auto currentFrame = (float)glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;
        int currentFPS;
        if (!fpscount)
        {
            currentFPS = (int)(1.0f / gDeltaTime);
            fpscount=5;
        }
        else
            fpscount--;
        shipVelocity = gDeltaTime * gCamera.MovementSpeed / 1.5f * 2;

        // Handle user input
        processInput(window);

        // Update wave data
        if (!gPause) {
            ocean.generateWave((float) glfwGetTime());
        }

        // All the rendering starts from here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up view and projection matrix
        glm::mat4 view = gCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom),
                                                (float)gScreenWidth / gScreenHeight, 0.1f, 10000.0f);

        skybox.Draw(skyboxShader, view, projection);

        glm::mat4 view_tmp = glm::lookAt(glm::vec3(35.9971, 141.684, 76.8194), glm::vec3(35.9971, 141.684, 76.8194) + glm::vec3(0.155126, - 0.466387, - 0.870873), glm::vec3(0.0817885, 0.884581, - 0.459159)); // (wocao[6], wocao[7], wocao[8]));
        shipshader.use();
        shipshader.setMat4("projection", projection);
        shipshader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));	// it's a bit too big for our scene, so scale it down
        model = glm::translate(model, shipPosition); // glm::vec3(-100.0f, 0.0f, 100.0f));
        //std::cout << shipPosition.x << " " << shipPosition.y << " " <<shipPosition.z << std::endl;
        model = glm::rotate(model, shipForward, glm::vec3(0.0f, 1.0f, 0.0f));
        shipshader.setMat4("model", model);
        ship.Draw(shipshader);
        
        {
            shader.use();
            // Set vertex shader data
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setMat4("model", glm::mat4(1.0f));
            shader.setFloat("time", (float)glfwGetTime());
            // Set fragment shader data
            shader.setVec3("viewPos", gCamera.Position);
            shader.setVec3("lightDir", glm::vec3(-1.0f, 1.0f, -1.0f));
            shader.setVec3("lightPos", glm::vec3(-1000.0f, -1000.0f, 5000.0f));
            shader.setVec3("diffuse", deepWaterColorSunset);
            shader.setVec3("ambient", deepWaterColorSunset);
            shader.setVec3("specular", glm::vec3(1.0f, 1.0f, 1.0f));
            shader.setInt("heightMap", 0);
            shader.setInt("normalMap", 1);
            shader.setInt("skybox", 2);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ocean.heightMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, ocean.normalMap);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubeMap());
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, ocean.indexCount, GL_UNSIGNED_INT, nullptr);
        }
        // Draw normals for debugging
        if (gDrawNormals) {
            normalShader.use();
            normalShader.setMat4("view", view);
            normalShader.setMat4("projection", projection);
            normalShader.setMat4("model", glm::mat4(1.0f));
            normalShader.setInt("heightMap", 0);
            normalShader.setInt("normalMap", 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ocean.heightMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, ocean.normalMap);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, ocean.indexCount, GL_UNSIGNED_INT, nullptr);
        }

        // Render XYZ coordinate
        // renderCoordinates(view, projection);

        // Render Texture to scene for debugging

        {
            // Start to render texts
            textRenderer.projection = glm::ortho(0.0f, (float)gScreenWidth,
                0.0f, (float)gScreenHeight);
            textRenderer.renderText(textShader, "FPS: " + std::to_string(currentFPS),
                0.0f, 280.0f, 0.3f,//gScreenHeight - 480*0.3f, 0.3f,
                glm::vec3(0.0, 1.0f, 1.0f));
            textRenderer.renderText(textShader, "Use WSAD to move, mouse to look around",
                0.0f, 2.0f, 0.3f,
                glm::vec3(0.0, 1.0f, 1.0f));
            textRenderer.renderText(textShader, "Use Q to switch between polygon and fill mode",
                0.0f, 18.0f, 0.3f,
                glm::vec3(0.0, 1.0f, 1.0f));
            textRenderer.renderText(textShader, "Use E to decide whether to draw normals",
                0.0f, 34.0f, 0.3f,
                glm::vec3(0.0, 1.0f, 1.0f));
            textRenderer.renderText(textShader, "Press P to pause or resume",
                0.0f, 50.0f, 0.3f,
                glm::vec3(0.0, 1.0f, 1.0f));
        }
        // Rendering Ends here

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow *init()
{
    // Initialization of GLFW context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Something needed for Mac OS X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // For anti-aliasing effects
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a window object
    GLFWwindow *window = glfwCreateWindow(gScreenWidth, gScreenHeight, "Ocean", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD before calling OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Tell OpenGL the size of rendering window
    glViewport(0, 0, gScreenWidth * 2, gScreenHeight * 2);

    // Set the windows resize callback function
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // Set up mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    return window;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    gScreenWidth = width;
    gScreenHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Handle camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);// , shipPosition );
        shipForwardxz.x = sin(shipForward);
        shipForwardxz.z = cos(shipForward);
        shipPosition += shipForwardxz * shipVelocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);// , shipPosition);
        shipForwardxz.x = sin(shipForward);
        shipForwardxz.z = cos(shipForward);
        shipPosition -= shipForwardxz * shipVelocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        shipForward += 0.001;
        relative.x = -sin(shipForward);
        relative.z = -cos(shipForward);
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);// , shipPosition + relative);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        shipForward -= 0.001;
        relative.x = -sin(shipForward);
        relative.z = -cos(shipForward);
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);//, shipPosition + relative);
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        std::cout << gCamera.Position.x << " " << gCamera.Position.y << " " << gCamera.Position.z << std::endl;
        std::cout << gCamera.Front.x << " " << gCamera.Front.y << " " << gCamera.Front.z << std::endl;
        std::cout << gCamera.Up.x << " " << gCamera.Up.y << " " << gCamera.Up.z << std::endl;
    }
    // Some mode switches
    static bool isPolygon = false;
    static double lastPressedTime = 0.0;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        if (isPolygon) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            isPolygon = false;
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            isPolygon = true;
        }
        lastPressedTime = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        gDrawNormals = !gDrawNormals;
        lastPressedTime = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        gPause = !gPause;
        lastPressedTime = glfwGetTime();
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    // Variables needed to handle mouse input
    static float lastMouseX = 400.0f;
    static float lastMouseY = 300.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;
        firstMouse = false;
    }

    // Calculate mouse movement since last frame
    float offsetX = (float)xpos - lastMouseX;
    float offsetY = (float)ypos - lastMouseY;
    lastMouseX = (float)xpos;
    lastMouseY = (float)ypos;

    gCamera.ProcessMouseMovement(offsetX, -offsetY);
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
    gCamera.ProcessMouseScroll((float)offsetY);
}

//void renderCoordinates(const glm::mat4 &view, const glm::mat4 &proj)
//{
//    static const glm::vec3 x(1.0f, 0.0f, 0.0f);
//    static const glm::vec3 y(0.0f, 1.0f, 0.0f);
//    static const glm::vec3 z(0.0f, 0.0f, 1.0f);
//    static unsigned int VAO = 0, VBO = 0;
//    static Shader shader("shaders/SingleColor.vert", "shaders/SingleColor.frag");
//    static const float vertices[] = {
//            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
//            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
//    };
//    if (VAO == 0 || VBO == 0) {
//        glGenBuffers(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glBindVertexArray(VAO);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
//        glEnableVertexAttribArray(1);
//    }
//    shader.use();
//    shader.setMat4("view", view);
//    shader.setMat4("projection", proj);
//    shader.setMat4("model", glm::mat4(1.0f));
//    glBindVertexArray(VAO);
//    shader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
//    glDrawArrays(GL_LINES, 0, 2);
//    shader.setVec4("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
//    glDrawArrays(GL_LINES, 2, 2);
//    shader.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
//    glDrawArrays(GL_LINES, 4, 2);
//}