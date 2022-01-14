#include <iostream>
#include <algorithm>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Shader.h>
#include <Camera.h>
#include <Model.h>

#include "Skybox.h"
#include "TextRenderer.h"
#include "VertexBufferOcean.h"

GLFWwindow *init();
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
unsigned int loadCubemap(std::vector<std::string> faces);
void getLightPos();

#define PI (acos(-1))
int ScreenWidth = 800;
int ScreenHeight = 600;
float DeltaTime = 0.0f;
float LastFrame = 0.0f;
bool blinn = false;
bool blinnKeyPressed = false;

Camera camera;
bool DrawNormals = false;
bool Pause = false;

glm::vec3 shipPosition(-500.0f, -70.3f, 300.0f);
float shipForward = 0;
glm::vec3 shipForwardxz(cos(shipForward), 0, sin(shipForward));
float shipVelocity;
glm::vec3 relative(12, 50, -50);
float angle = 0;

glm::vec3 lightPos(0.0f, 0.5f, 0.0f);
float light_radius = 0.5f;
float light_angle = 90.0f * PI / 180;

float skyboxVertices[] = {        
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,

    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f
};

int main()
{
    GLFWwindow *window = init();
    if (window == nullptr) {
        std::cout << "Failed to initialize GLFW and OpenGL!" << std::endl;
        return -1;
    }

    // Load shaders
    Shader shader("./shaders/Water.vert", "./shaders/Water.frag");
    Shader textShader("./shaders/TextShader.vert", "./shaders/TextShader.frag");
    Shader normalShader("./shaders/DrawNormal.vert", "./shaders/DrawNormal.frag",
                        "./shaders/DrawNormal.geom");

    Model ship("./model/Fishing_boat/Boat.obj");
    Shader shipshader("./model/Fishing_boat/boat.vs", "./model/Fishing_boat/boat.fs");

    // Initialize fonts
    TextRenderer textRenderer;
    
    std::vector<std::string> faces
    {
        "./textures/skybox/right.jpg",
        "./textures/skybox/left.jpg",
        "./textures/skybox/top.jpg",
        "./textures/skybox/bottom.jpg",
        "./textures/skybox/front.jpg",
        "./textures/skybox/back.jpg",
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    Shader skyboxShader("./shaders/skybox.vs", "./shaders/skybox.fs");
    
    // Necessary OpenGL Parameters
    glEnable(GL_DEPTH_TEST);
    // Enable gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);
    // Enable anti-aliasing
    glEnable(GL_MULTISAMPLE);
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera.Position = glm::vec3(0.0f, 20.0f, 40.0f);

    VertexBufferOcean ocean(glm::vec2(2.0f, 2.0f), 128, 0.02f);
    ocean.generateWave((float)glfwGetTime());
    // Pass the vertex data to GPU
    unsigned int VBO, VBO2, EBO, VAO;
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.vertexCount, ocean.vertices, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &VBO2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.normalCount, ocean.normals, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ocean.indexCount, ocean.indices, GL_STATIC_DRAW);
    }
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    {
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
    

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    short acount = 0;
    while (!glfwWindowShouldClose(window)) {
        // Calculate how much time since last frame
        auto currentFrame = (float)glfwGetTime();
        DeltaTime = currentFrame - LastFrame;
        LastFrame = currentFrame;
        auto currentFPS = (int)(1.0f / DeltaTime);
        shipVelocity = DeltaTime * camera.MovementSpeed * 80.0f * 4.0f;

        // Handle user input
        processInput(window);

        // Update wave data
        if (!Pause) {
            ocean.generateWave((float)glfwGetTime());
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.vertexCount, ocean.vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.normalCount, ocean.normals, GL_DYNAMIC_DRAW);

        // All the rendering starts from here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up view and projection matrix
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)ScreenWidth / ScreenHeight, 0.1f, 5000.0f);

        shader.use();
        // Set vertex shader data
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        // Set fragment shader data
        shader.setVec3("cameraPos", camera.Position);
        // set light uniforms
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        // light properties
        glm::vec3 ambient(0.2f, 0.2f, 0.2f);
        glm::vec3 diffuse(1.0f, 1.0f, 1.0f);
        glm::vec3 specular(1.0f, 1.0f, 1.0f);
        shader.setVec3("light.ambient", ambient);
        shader.setVec3("light.diffuse", diffuse);
        shader.setVec3("light.specular", specular);
        shader.setInt("blinn", blinn);
        glm::vec3 basecolor(0.1f, 0.2f, 0.35f);
        shader.setVec3("basecolor", basecolor);

        glBindVertexArray(VAO);

        shader.setInt("skybox", 0);
        glm::mat4 omodel;
        glDrawElements(GL_TRIANGLES, ocean.indexCount, GL_UNSIGNED_INT, nullptr);
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                omodel = glm::mat4(1.0f);
                omodel = glm::rotate(omodel, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                omodel = glm::translate(omodel, glm::vec3(47.50f * (i-10), 0.0f, 47.50f * (j-10)));
                shader.setMat4("model", omodel);
                glDrawElements(GL_TRIANGLES, ocean.indexCount, GL_UNSIGNED_INT, nullptr);
            }
        }
        glBindVertexArray(0);


        shipshader.use();
        shipshader.setMat4("projection", projection);
        shipshader.setMat4("view", view);

        if (!acount)
        {
            if (angle > 0.01)
                angle -= (rand() / (double)RAND_MAX) / 20;
            else if (angle < -0.01)
                angle += (rand() / (double)RAND_MAX) / 20;
            else
                angle += (rand() / (double)RAND_MAX - 0.5f) / 10;
            acount = 3;
        }
        else
        {
            acount--;
        }
        shipPosition.y += rand()/(double)RAND_MAX  / 2 - 0.25f;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));	// it's a bit too big for our scene, so scale it down
        model = glm::translate(model, shipPosition);
        model = glm::rotate(model, shipForward, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, angle, shipForwardxz);
        shipshader.setMat4("model", model);
        ship.Draw(shipshader);
        // Draw normals for debugging
        if (DrawNormals) {
            normalShader.use();
            normalShader.setMat4("view", view);
            normalShader.setMat4("projection", projection);
            normalShader.setMat4("model", glm::mat4(1.0f));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, ocean.indexCount, GL_UNSIGNED_INT, nullptr);
        }

         //draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        
        // Start to render texts
        textRenderer.projection = glm::ortho(0.0f, (float)ScreenWidth,
                                             0.0f, (float)ScreenHeight);
        textRenderer.renderText(textShader, "FPS: " + std::to_string(currentFPS),
                                340.0f, 260.0f, 0.3f,
                                glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use WSAD to move, mouse to look around",
                                0.0f, 289.0f, 0.3f,
                                glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use Q to switch between polygon and fill mode",
                                0.0f, 275.0f, 0.3f,
                                glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use V to increase the wind speed",
                                0.0f, 260.0f, 0.3f,
                                glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use B to decrease the wind speed",
                                0.0f, 245.0f, 0.3f,
                                glm::vec3(1.0, 0.0f, 0.0f));
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
    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Water", nullptr, nullptr);
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
    glViewport(0, 0, ScreenWidth * 2, ScreenHeight * 2);

    // Set the windows resize callback function
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // Set up mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    ScreenWidth = width;
    ScreenHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Handle camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, DeltaTime);
        shipPosition += shipForwardxz * shipVelocity;
        /*std::cout << shipForward << std::endl;
        std::cout << shipForwardxz.x << " " << shipForwardxz.z << std::endl;
        std::cout << shipPosition.x << " " << shipPosition.y << " " << shipPosition.z << std::endl;*/

    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, DeltaTime);
        shipPosition -= shipForwardxz * shipVelocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, DeltaTime);
        shipForward += 0.0015 * 4.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, DeltaTime);
        shipForward -= 0.0015 * 4.0f;
    }
    shipForwardxz.x = -cos(shipForward);
    shipForwardxz.z = sin(shipForward);

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
        DrawNormals = !DrawNormals;
        lastPressedTime = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        Pause = !Pause;
        lastPressedTime = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        light_angle -= 2.0 / 90;
        if (light_angle < 15.0 * PI / 180) {
            light_angle = 15.0 * PI / 180;
        }
        getLightPos();
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        light_angle += 2.0 / 90;
        if (light_angle > 155.0 * PI / 180) {
            light_angle = 155.0 * PI / 180;
        }
        getLightPos();
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

    camera.ProcessMouseMovement(offsetX, -offsetY);
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
    camera.ProcessMouseScroll((float)offsetY);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{   
    GLint last_texture_ID;
    GLuint textureID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    return textureID;
}

void getLightPos()
{
    float x = light_radius * cos(light_angle);
    float y = light_radius * sin(light_angle);
    lightPos = glm::vec3(x, y, 0.0);
}