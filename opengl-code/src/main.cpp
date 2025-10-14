#include "../lib/glad/glad.h"
#include <GLFW/glfw3.h>

// #include "../lib/glm/glm.hpp"
// #include "../lib/glm/gtc/matrix_transform.hpp"
// #include "../lib/glm/gtc/type_ptr.hpp"

#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/skybox.h"
#include "../include/model.h"

#include <cstdio>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <string>


//global constants 
using Skybox = Learus_Skybox::Skybox;
bool animation = false;

// Timing
float lastFrame = 0.0f;
float frameToggled = 0.0f;
float timeSinceLastToggle = 1.0f;

// Some settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float earthOrbitRadius = 15.0f;
glm::vec3 centerPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 earthPos = centerPos + glm::vec3(sin(frameToggled) * earthOrbitRadius, 0.0f, cos(frameToggled) * earthOrbitRadius);

// Camera
glm::vec3 ref_cam_pos = glm::vec3(0.0f, 0.0f, 20.0f);
glm::vec3 initial_cam_pos = glm::vec3(0.0f, 0.0f, 20.0f);
Camera camera(initial_cam_pos);
float cameraOrbitRadius = 15.0f;
float rotateAngle = 1.f;

// Mouse Input
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const char* glsl_version = "#version 130";

glm::vec3 pointLightPositions[] = {
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
}; 

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseInput(GLFWwindow * window, double xpos, double ypos);
void scrollInput(GLFWwindow * window, double xoffset, double yoffset);
void keyboardInput(GLFWwindow * window, float deltaTime);

void reset_cam_pos(){
    camera.Position = ref_cam_pos;
    camera.Yaw = -90;
    camera.Pitch = 0.0;
    camera.updateCameraVectors(); 
}

void render_main_menu_bar(){

    if (ImGui::BeginMainMenuBar()){
        if (ImGui::BeginMenu("Actions")) {
            if (ImGui::MenuItem("Reset camera position")){
                reset_cam_pos();  
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// Handles user keyboard input. Supposed to be used every frame, so deltaTime can be calculated appropriately.
void keyboardInput(GLFWwindow * window, float deltaTime){
    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);    

    if (glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
        camera.Move(Camera_Movement::FORWARD, deltaTime);

    if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
        camera.Move(Camera_Movement::BACKWARD, deltaTime);

    if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
        camera.Move(Camera_Movement::LEFT, deltaTime);

    if (glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
        camera.Move(Camera_Movement::RIGHT, deltaTime);

    if (glfwGetKey(window,GLFW_KEY_U) == GLFW_PRESS)
        camera.Move(Camera_Movement::UP, deltaTime);

    if (glfwGetKey(window,GLFW_KEY_I) == GLFW_PRESS)
        camera.Move(Camera_Movement::DOWN, deltaTime);

    // Upwards Rotation
    if (GLFW_MOD_CONTROL && glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
        camera.Orbit(UP, cameraOrbitRadius, rotateAngle);

    // Downwards Rotation
    if (GLFW_MOD_CONTROL && glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Orbit(DOWN, cameraOrbitRadius, rotateAngle);

    // Rightwards Rotation
    if (GLFW_MOD_CONTROL && glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Orbit(RIGHT, cameraOrbitRadius, rotateAngle);

    // Leftwards Rotation
    if (GLFW_MOD_CONTROL && glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Orbit(LEFT, cameraOrbitRadius, rotateAngle);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        reset_cam_pos();
    }

    std::cout << "cam pos: " << camera.Position.x << "," << camera.Position.y << "," << camera.Position.z << std::endl;
    std::cout << "cam front vector: " << camera.Front.x << "," << camera.Front.y << "," << camera.Front.z << std::endl;  
    std::cout << "earth pos: " << earthPos.x << "," << earthPos.y << "," << earthPos.z << std::endl;  
}

// Handles mouse scroll wheel. Supposed to be used as the glfw scroll callback.
void scrollInput(GLFWwindow* window, double xoffset, double yoffset){
    camera.Zoom(yoffset);
}

//skyBox.setUniforms(projection, glm::mat4(glm::mat3(view)));
//skyBox.Draw();

int main(){
    // Initialize and configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);
    if (window == NULL){
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }        

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollInput);

    // Load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //initialize imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable
                                | ImGuiConfigFlags_NavEnableKeyboard
                                | ImGuiConfigFlags_NavEnableSetMousePos;
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //load vertex and fragment shaders 
    Shader planetShader("./shaders/planet.vs", "./shaders/planet.fs");    
    // Load the models
    Model Earth("./models/immune_system/Globe.obj");
    
    Skybox skyBox("./images/top.png", "./images/bottom.png", "./images/left.png", "./images/right.png", "./images/front.png", "./images/back.png");

    // Render Loop
    while(!glfwWindowShouldClose(window)) {                        
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        timeSinceLastToggle += deltaTime;
        if (animation)
            frameToggled += deltaTime;

        keyboardInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        

        //imgui + opengl integration (frame initialization)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();        
        
        //draw main menu bar 
        render_main_menu_bar(); 
        
        // view / projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        planetShader.use();

        // Set the lighting
        planetShader.setVec3("viewPos", camera.Position);
        planetShader.setFloat("material.shininess", 32.0f);

        planetShader.setVec3("dirLight.position", camera.Position);
        planetShader.setVec3("dirLight.ambient", 1.f, 1.f, 1.f);
        planetShader.setVec3("dirLight.diffuse", 1.5f, 1.5f, 1.5f);
        planetShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        
        std::string uniform_name = "pointLights[";
        for (int i = 0; i < 4; i++) {
            uniform_name += std::to_string(i) + "]";
            std::string pos_name = uniform_name + ".position";
            planetShader.setVec3(pos_name, pointLightPositions[i]);
            planetShader.setVec3(uniform_name + ".ambient", 1.f, 1.f, 1.f);
            planetShader.setVec3(uniform_name + ".diffuse", 1.f, 1.f, 1.f);
            planetShader.setVec3(uniform_name + ".specular", 1.f, 1.f, 1.f);
            planetShader.setFloat(uniform_name + ".constant", 1.0f);
            planetShader.setFloat(uniform_name + ".linear", 0.045f);
            planetShader.setFloat(uniform_name + ".quadratic", 0.0075f);
        }
        
        // Render the Earth object
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        // Orbit around the sun
        earthPos = centerPos + glm::vec3(sin(frameToggled) * earthOrbitRadius, 0.0f, cos(frameToggled) * earthOrbitRadius);
        model = glm::translate(model, earthPos);
        // Rotate around itself
        model = glm::rotate(model, frameToggled * 1.5f * glm::radians(-50.0f), glm::vec3(0.1f, 1.0f, 0.0f));

        planetShader.setMat4("model", model);
        Earth.Draw(planetShader);         

        ImGui::SetNextWindowSize(ImVec2(300, 300));
        ImGui::SetNextWindowPos(ImVec2(0, 20));
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());            
            
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/*
        // ImGui::Begin("Scene", &show_dialog);
        // //{
        // ImGui::BeginChild("toolbar", {0.0f, 30.0f});
        //     //ImGui::BeginChildFrame(ImGui::GetID("toolbar"), ImVec2(300, 300)); 
        // {
        //     if (ImGui::Button("Add Component")) {
        //         std::cout << "to do" << std::endl;
        //     }
        //     ImGui::SameLine();
        //        // ImGui::EndChildFrame();                
        //     //}            
        // } 
        // ImGui::EndChild();
        // ImGui::End();
*/