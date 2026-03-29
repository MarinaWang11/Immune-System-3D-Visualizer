#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>             
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>         

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>
#include <sys/types.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h> 
#include <ctime> 
#include <sys/stat.h>
#include <algorithm>
#include <limits> 
#include <set>
#include <map>

#include <filesystem>
namespace fs = std::filesystem;

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

#include "../include/shader.h" 
#include "../include/camera.h"
#include "../include/Sphere.h"
#include "../include/Cube.h"
#include "../include/Cylinder.h"
#include "../include/Colors.h" 

struct CellData{
    glm::vec3 position; 
    std::string type;  
    std::string state;  
};

struct SimFile {
    std::string path;
    float timeValue; // Num do arquivo 
    
    // Ordenar os arquivos do menor tempo para o maior
    bool operator<(const SimFile& other) const {
        return timeValue < other.timeValue;
    }
};


// VARIÁVEIS GLOBAIS 
std::vector<SimFile> simulationFiles; //Lista de arquivos 
std::string currentSimFolder = "data";
std::string detectedPrefix = "field_"; // Valor padrão inicial
int selectedCellIndex = -1; 
bool mousePressedLastFrame = false; 

bool isCameraMode = true; 
float animationTime = 0.0f; 
bool requestSnapshot = false; 
bool showGrid = true;
bool showDataTable = true; 

// VARIÁVEIS DE ANIMAÇÃO 
bool isPlaying = false;          // Estado: Toca ou Pausa
float timeAccumulator = 0.0f;    // Acumula o tempo passado
float stepDelay = 0.2f;          // Tempo entre passos (0.2s = 5 passos por segundo)
float currentSimStep = 0;
float maxSimStepsFound = 0; 

const float UI_PANEL_WIDTH = 300.0f;
const float DETAILS_PANEL_HEIGHT = 160.0f;

Cube* visualCube = nullptr;
Cylinder* visualCylinder = nullptr;

std::map<std::string, std::set<std::string>> activeAgents;
std::vector<CellData> cells;



void parseCSV(const std::string& filename){
    cells.clear();
    activeAgents.clear();
    selectedCellIndex = -1; 

    std::ifstream file(filename); 
    if(!file.is_open()){
        return;
    }
    
    std::string line;
    std::getline(file, line); // Pula cabeçalho

    while (std::getline(file, line)){
        std::stringstream ss(line); 
        std::string value;
        CellData cell;
        if(!(std::getline(ss, value, ','))) break;
        cell.position.x = std::stof(value);
        std::getline(ss, value, ','); cell.position.y = std::stof(value);
        std::getline(ss, value, ','); cell.position.z = std::stof(value);
        std::getline(ss, value, ','); cell.type = value;
        std::getline(ss, value, ','); cell.state = value;
        
        if(cell.type != "Empty"){
            cells.push_back(cell);
            activeAgents[cell.type].insert(cell.state);
        }
    }

    file.close();
}

void updateSimulationFolder(std::string path){
    std::cout << "[DEBUG] Recebi o pedido para abrir: " << path << std::endl;
    fs::path p(path);

    if(!fs::exists(p) || !fs::is_directory(p)){
        std::cout << "[ERRO] Pasta invalida ou nao existe: " << path << std::endl;
        return;
    }

    currentSimFolder = p.string(); 
    simulationFiles.clear(); // Limpa a lista antiga
    
    
    detectedPrefix = "field_"; 

    for (const auto& entry : fs::directory_iterator(p)){
        if(entry.path().extension() == ".csv"){
            std::string name = entry.path().filename().string();
            if(name.find("field_") == 0){ detectedPrefix = "field_"; break; }
            if(name.find("Field_") == 0){ detectedPrefix = "Field_"; break; }
            if(name.find("data_") == 0){ detectedPrefix = "data_"; break; }
        }
    }
    
    // lê  arquivos, extrai o num e guarda na lista
    for (const auto& entry : fs::directory_iterator(p)){
        if(entry.path().extension() == ".csv"){
            std::string name = entry.path().filename().string();
            
            if(name.find(detectedPrefix) == 0) {
                std::string numStr = name.substr(detectedPrefix.length());
                numStr = numStr.substr(0, numStr.find(".csv"));
                
                try {
                    float timeVal = std::stof(numStr); // Converte "0.5" para float

                    simulationFiles.push_back({entry.path().string(), timeVal});
                } catch (...) {
                    // Ignora arquivos que não tenham um número válido no nome
                }
            }
        }
    }

    // Ordena os arquivos 
    std::sort(simulationFiles.begin(), simulationFiles.end());

    maxSimStepsFound = simulationFiles.size();
    
    //Carrega o primeiro arquivo
    if(maxSimStepsFound > 0){
        currentSimStep = 0; // ID da lista
        parseCSV(simulationFiles[currentSimStep].path);

        std::cout << "[SUCESSO] Carregados " << maxSimStepsFound << " arquivos." << std::endl;
    } else {
        std::cout << "[AVISO] Pasta valida, mas sem arquivos CSV compativeis." << std::endl;
    }
}

// Função para salvar PNG
void saveSnapshot(GLFWwindow* window){
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    
    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;

    std::vector<char> buffer(bufferSize);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    stbi_flip_vertically_on_write(true);
    
    struct stat st ={0};
    if(stat("print", &st) == -1) MKDIR("print");
    
    std::time_t now = std::time(nullptr);
    std::string filename = "print/snapshot_" + std::to_string(now) + ".png";

    if(stbi_write_png(filename.c_str(), width, height, nrChannels, buffer.data(), stride)){
        std::cout << "Snapshot salvo: " << filename << std::endl;
    }
}

ImVec2 worldToScreen(glm::vec3 pos, glm::mat4 view, glm::mat4 projection, int width, int height, float xOffset){
    glm::vec4 clipSpace = projection * view * glm::vec4(pos, 1.0f);

    if(clipSpace.w <= 0.0f) return ImVec2(-10000, -10000); 

    glm::vec3 ndcSpace = glm::vec3(clipSpace) / clipSpace.w;

    float renderWidth = (float)width - xOffset;
    float x = ((ndcSpace.x + 1.0f) / 2.0f) * renderWidth + xOffset;
    float y = ((1.0f - ndcSpace.y) / 2.0f) * height; 

    return ImVec2(x, y);
}

bool raySphereIntersect(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 sphereCenter, float sphereRadius, float& distance){
    glm::vec3 oc = rayOrigin - sphereCenter;

    float b = glm::dot(oc, rayDir);
    float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
    float delta = b * b - c;

    if(delta < 0.0f) return false;

    float sqrtDelta = sqrt(delta);
    float t = -b - sqrtDelta; 

    if(t < 0.0f) t = -b + sqrtDelta; 
    if(t < 0.0f) return false; 

    distance = t;

    return true;
}

glm::vec3 getRayFromMouse(float mouseX, float mouseY, int screenW, int screenH, glm::mat4 projection, glm::mat4 view, float xOffset){
    float renderWidth = (float)screenW - xOffset;
    float x = (2.0f * (mouseX - xOffset)) / renderWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / (float)screenH; 

    glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;

    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

    glm::vec3 ray_wor = glm::vec3(glm::inverse(view) * ray_eye);

    return glm::normalize(ray_wor);
}

void renderCube(Shader &shader, glm::vec3 pos, glm::vec3 color, float alpha, float length, float width, float height) {
    shader.use();
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, glm::vec3(length, height, width)); 
    
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    shader.setFloat("alpha", alpha); 
    
    visualCube->draw();
}

void renderCylinder(Shader &shader, glm::vec3 pos, glm::vec3 color, float alpha, float radius, float height) {
    if (visualCylinder == nullptr) return;

    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    
    model = glm::scale(model, glm::vec3(radius * 2.0f, height, radius * 2.0f)); 
    
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    shader.setFloat("alpha", alpha); 
    
    visualCylinder->draw();
}

unsigned int gridVAO, gridVBO;
int gridVertexCount = 0;

void setupGridAndAxes(){
    std::vector<float> v;
    int size = 15;

    for(int i = 0; i <= size; ++i){
        v.insert(v.end(),{ (float)i, 0.0f, 0.0f,  0,0,0, 0,0 }); v.insert(v.end(),{ (float)i, (float)size, 0.0f,  0,0,0, 0,0 });
        v.insert(v.end(),{ 0.0f, (float)i, 0.0f,  0,0,0, 0,0 }); v.insert(v.end(),{ (float)size, (float)i, 0.0f,  0,0,0, 0,0 });
        v.insert(v.end(),{ (float)i, 0.0f, 0.0f,  0,0,0, 0,0 }); v.insert(v.end(),{ (float)i, 0.0f, (float)size,  0,0,0, 0,0 });
        v.insert(v.end(),{ 0.0f, 0.0f, (float)i,  0,0,0, 0,0 }); v.insert(v.end(),{ (float)size, 0.0f, (float)i,  0,0,0, 0,0 });
    }

    v.insert(v.end(),{ 0.0f, 0.0f, 0.0f, 0,0,0, 0,0 }); v.insert(v.end(),{ 17.0f, 0.0f, 0.0f, 0,0,0, 0,0 }); 
    v.insert(v.end(),{ 0.0f, 0.0f, 0.0f, 0,0,0, 0,0 }); v.insert(v.end(),{ 0.0f, 17.0f, 0.0f, 0,0,0, 0,0 }); 
    v.insert(v.end(),{ 0.0f, 0.0f, 0.0f, 0,0,0, 0,0 }); v.insert(v.end(),{ 0.0f, 0.0f, 17.0f, 0,0,0, 0,0 }); 

    gridVertexCount = v.size() / 8;

    glGenVertexArrays(1, &gridVAO); glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO); glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
}

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 initialPos(32.0f, 28.0f, 32.0f);

Camera camera(initialPos); 

float deltaTime = 0.0f; 
float lastFrame = 0.0f;

void framebufferSizeCallback(GLFWwindow* window, int width, int height){ glViewport(0, 0, width, height); }

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    if(ImGui::GetIO().WantCaptureMouse) return; 

    camera.Zoom(static_cast<float>(yoffset)); 
}

void processInput(GLFWwindow * window); 

int main(){
    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Visualizador Celular 3D - Linha do Tempo", NULL, NULL);

    if(!window){ 
        glfwTerminate(); 
        return -1; 
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
  
    IMGUI_CHECKVERSION();
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    camera.Yaw = -135.0f; camera.Pitch = -30.0f; camera.updateCameraVectors();
    glEnable(GL_DEPTH_TEST); 
    Shader cellShader("./shaders/planet.vs", "./shaders/planet.fs"); 
    
    
    Sphere sphere(0.5f, 24, 18, true); 
    setupGridAndAxes();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    visualCube = new Cube();
    visualCylinder = new Cylinder(36);
    if (visualCylinder != nullptr) {
        std::cout << "Cilindro inicializado com sucesso!" << std::endl;
    }
    std::string pathInicial = fs::current_path().string() + "/data";
    
    if(fs::exists(pathInicial)){
        updateSimulationFolder(pathInicial);
    } else {
        updateSimulationFolder(fs::current_path().string());
    }

    while(!glfwWindowShouldClose(window)){
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if(isPlaying && maxSimStepsFound > 0){
            timeAccumulator += deltaTime;
            
            if(timeAccumulator >= stepDelay){
                timeAccumulator = 0.0f; 
                
                currentSimStep ++;
                
                if(currentSimStep >= maxSimStepsFound){
                    currentSimStep = 0;
                }
                
                parseCSV(simulationFiles[currentSimStep].path);
            }
        }

        if(!isCameraMode) animationTime += deltaTime;
        processInput(window);

        int sw, sh; glfwGetWindowSize(window, &sw, &sh);

        bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if(mousePressed && !mousePressedLastFrame && !io.WantCaptureMouse){
            double mx, my; glfwGetCursorPos(window, &mx, &my);

            if(mx > UI_PANEL_WIDTH + 15){ 
                glm::mat4 proj = glm::perspective(glm::radians(camera.zoom), (float)(sw - UI_PANEL_WIDTH - 15) / (float)sh, 0.1f, 1000.0f);
                glm::mat4 view = camera.GetViewMatrix();
                glm::vec3 rO = camera.Position;
                glm::vec3 rD = getRayFromMouse((float)mx, (float)my, sw, sh, proj, view, UI_PANEL_WIDTH + 15);

                float minD = std::numeric_limits<float>::max();
                int hitIdx = -1;

                for(size_t i = 0; i < cells.size(); ++i){
                    glm::mat4 m = glm::rotate(glm::mat4(1.0f), animationTime * 0.5f, glm::vec3(0, 1, 0));
                    m = glm::translate(m, cells[i].position);
                    glm::vec3 pos = glm::vec3(m[3]);
                    float d;

                    if(raySphereIntersect(rO, rD, pos, 0.4f, d)){ 
                        if(d < minD){ 
                            minD = d; hitIdx = (int)i;
                        } 
                    }
                }

                if(hitIdx != -1){
                    selectedCellIndex = (selectedCellIndex == hitIdx) ? -1 : hitIdx;
                } else {
                    selectedCellIndex = -1;
                }
            }
        }
        mousePressedLastFrame = mousePressed;

        glClearColor(0.95f, 0.95f, 0.95f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

        //  RENDERIZAÇÃO 3D 
        glViewport((int)UI_PANEL_WIDTH + 15, 0, sw - (int)UI_PANEL_WIDTH - 15, sh);
        cellShader.use();
        cellShader.setFloat("alpha", 0.9f);
        cellShader.setVec3("viewPos", camera.Position); 
        cellShader.setFloat("shininess", 32.0f); 
        
        SceneLighting light = getGlobalLighting();
        cellShader.setVec3("dirLight.direction", light.direction);
        cellShader.setVec3("dirLight.ambient", light.ambient);   
        cellShader.setVec3("dirLight.diffuse", light.diffuse);     
        cellShader.setVec3("dirLight.specular", light.specular);

        for(int i = 0; i < 4; i++){
             std::string n = "spotLights[" + std::to_string(i) + "]";
             cellShader.setVec3(n + ".ambient", 0.1f, 0.1f, 0.1f);
             cellShader.setVec3(n + ".diffuse", 0.0f, 0.0f, 0.0f);
             cellShader.setFloat(n + ".constant", 1.0f);
             cellShader.setFloat(n + ".linear", 0.09f);
             cellShader.setFloat(n + ".quadratic", 0.032f);
        }

        glm::mat4 proj = glm::perspective(glm::radians(camera.zoom), (float)(sw - UI_PANEL_WIDTH - 15) / (float)sh, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        cellShader.setMat4("projection", proj); cellShader.setMat4("view", view);

        if(showGrid){
            cellShader.setMat4("model", glm::mat4(1.0f)); glBindVertexArray(gridVAO);
            cellShader.setVec3("dirLight.ambient", 1,1,1); cellShader.setVec3("dirLight.diffuse", 0,0,0);
            
            glLineWidth(1.0f); 
            cellShader.setVec3("objectColor", 1.0f, 0.0f, 0.0f); glDrawArrays(GL_LINES, gridVertexCount - 6, 2); 
            cellShader.setVec3("objectColor", 0.0f, 1.0f, 0.0f); glDrawArrays(GL_LINES, gridVertexCount - 4, 2); 
            cellShader.setVec3("objectColor", 0.0f, 0.0f, 1.0f); glDrawArrays(GL_LINES, gridVertexCount - 2, 2); 
            cellShader.setVec3("objectColor", 0.6f, 0.6f, 0.6f); glDrawArrays(GL_LINES, 0, gridVertexCount - 6); 
            glLineWidth(1.0f); 
            
            cellShader.setVec3("dirLight.ambient", light.ambient); cellShader.setVec3("dirLight.diffuse", light.diffuse); 
        }


        for(size_t i = 0; i < cells.size(); ++i){
            glm::vec3 color = getCellColor(cells[i].type, cells[i].state);
            if((int)i == selectedCellIndex) color = glm::vec3(1.0f, 1.0f, 0.0f); 
            
            cellShader.setVec3("objectColor", color);

            glm::mat4 m = glm::rotate(glm::mat4(1.0f), animationTime * 0.5f, glm::vec3(0, 1, 0));
            m = glm::translate(m, cells[i].position);
            m = glm::scale(m, glm::vec3(0.4f)); 

            cellShader.setMat4("model", m); 
            sphere.draw();
        }

        glDepthMask(GL_FALSE);

        // Teste: Um cubo vermelho translúcido         
        // Posição: x=10, y=5, z=10
        glm::vec3 posicaoCubo = glm::vec3(10.0f, 5.0f, 10.0f);

        // Cor: Vermelho (R=1, G=0, B=0)
        glm::vec3 corCubo = glm::vec3(1.0f, 0.0f, 0.0f);

        // renderCube(shader, pos, cor, ALPHA, comprimento(X), largura(Y), altura(Z))
        // renderCube(cellShader, posicaoCubo, corCubo, 0.3f, 8.0f, 8.0f, 8.0f);

        //renderCylinder(shader, pos, cor, ALPHA, raio, altura)
        //renderCylinder(cellShader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 5.0f, 20.0f);

        glDepthMask(GL_TRUE);

        glViewport(0, 0, sw, sh);

        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("Arquivo")){
                if(ImGui::MenuItem("Abrir Pasta de Simulação...")){
                    IGFD::FileDialogConfig config;
                    config.path = ".";

                    ImGuiFileDialog::Instance()->OpenDialog("ChooseDirKey", "Selecione a Pasta da Simulação", nullptr, config);
                }

                if(ImGui::MenuItem("Snapshot PNG")) requestSnapshot = true;
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Visualizar")){
                ImGui::MenuItem("Grade", NULL, &showGrid);
                ImGui::MenuItem("Tabela", NULL, &showDataTable);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Lógica de Processamento da Seleção
        if(ImGuiFileDialog::Instance()->Display("ChooseDirKey")){
            if(ImGuiFileDialog::Instance()->IsOk()){
                std::string finalPath = "";
                
                std::string currentPath = ImGuiFileDialog::Instance()->GetCurrentPath();
                
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

                if(!filePathName.empty() && fs::is_directory(fs::path(filePathName))){
                    finalPath = filePathName;
                } else {
                    finalPath = currentPath;
                }

                std::cout << "[DEBUG UI] Caminho bruto da interface: " << finalPath << std::endl;

                if(!finalPath.empty()){
                    updateSimulationFolder(finalPath);
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        //  INTERFACE LATERAL ESQUERDA 
        static std::string selectedTypeForPopup = "";
        
        ImGui::SetNextWindowPos(ImVec2(10, 40)); 
        ImGui::SetNextWindowSize(ImVec2(UI_PANEL_WIDTH, 0));
        ImGui::Begin("Legenda e Controles", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
            
            ImGui::Text("Simulacao:");
            ImGui::Separator();
            
            ImGui::Text("Pasta: %s", currentSimFolder.c_str());
            if(maxSimStepsFound > 0) {              
                ImGui::Text("Tempo da Simulacao: %.2f / %.2f", simulationFiles[currentSimStep].timeValue, simulationFiles[maxSimStepsFound - 1].timeValue);
            } else {
                ImGui::Text("Nenhum arquivo compativel carregado.");
            }

            if(ImGui::Button("-", ImVec2(40, 0))){ 
                if(currentSimStep > 0){ 
                    currentSimStep--; 

                    parseCSV(simulationFiles[currentSimStep].path); 
                } 
            }

            ImGui::SameLine();

            if(ImGui::Button("+", ImVec2(40, 0))){ 
                if(currentSimStep < maxSimStepsFound - 1){ 
                    currentSimStep++; 

                    parseCSV(simulationFiles[currentSimStep].path); 
                } 
            }

            ImGui::Separator();

            ImGui::Text("Animacao:");
            
            if(ImGui::Button(isPlaying ? "PAUSAR ||" : "TOCAR >", ImVec2(90, 30))){
                isPlaying = !isPlaying;
            }
            
            ImGui::SameLine();
            
            if(ImGui::Button("REINICIAR |<", ImVec2(100, 30))){
                isPlaying = false;      // Para a animação
                currentSimStep = 0;     // Volta para o passo 0
                parseCSV(simulationFiles[currentSimStep].path); // Carrega o arquivo 0
            }
            
            // Slider de Velocidade
            ImGui::Spacing();
            ImGui::Text("Velocidade (segundos):");
            ImGui::SliderFloat("##speed", &stepDelay, 0.05f, 2.0f, "%.2f s");
            
            ImGui::Separator();

            ImGui::Text("Agentes:");
            
            for(std::map<std::string, std::set<std::string>>::iterator it = activeAgents.begin(); it != activeAgents.end(); ++it){
                const std::string& type = it->first;
                glm::vec3 bCol = getCellColor(type, "Live");
                
                ImGui::PushID(type.c_str());
                ImGui::ColorButton("##color", ImVec4(bCol.r, bCol.g, bCol.b, 1.0f));
                ImGui::SameLine();
                if(ImGui::Selectable(type.c_str(), selectedTypeForPopup == type)){ 
                    selectedTypeForPopup = (selectedTypeForPopup == type) ? "" : type; 
                }
                ImGui::PopID();
            }
            
            ImGui::Separator();
            ImGui::ColorButton("##sel_main", ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); ImGui::SameLine(); ImGui::Text("Selecionado");
            
            ImGui::Separator();
            ImGui::TextDisabled("Atalhos:");
            ImGui::Text("[O] Orbita | [C] Cam Livre");
            ImGui::Text("[W,A,S,D] Mover | [Setas] Visao");
            ImGui::Text("[-][+] Tempo");
            
            ImVec2 mainWinPos = ImGui::GetWindowPos(); ImVec2 mainWinSize = ImGui::GetWindowSize();
        ImGui::End();

        //  SUB-LEGENDA DE ESTADOS 
        if(!selectedTypeForPopup.empty() && activeAgents.count(selectedTypeForPopup)){
            bool openStates = true;
            ImGui::SetNextWindowPos(ImVec2(mainWinPos.x + mainWinSize.x + 10, mainWinPos.y), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_Always);
            
            std::string subTitle = "Estados do Agente##StatesSubWindow";
            if(ImGui::Begin(subTitle.c_str(), &openStates, ImGuiWindowFlags_AlwaysAutoResize)){
                ImGui::Text("Tipo: %s", selectedTypeForPopup.c_str());
                ImGui::Separator();
                
                std::set<std::string>& states = activeAgents[selectedTypeForPopup];
                for(std::set<std::string>::iterator sit = states.begin(); sit != states.end(); ++sit){
                    glm::vec3 sCol = getCellColor(selectedTypeForPopup, *sit);

                    ImGui::PushID(sit->c_str());
                    ImGui::ColorButton("##s_color", ImVec4(sCol.r, sCol.g, sCol.b, 1.0f));
                    ImGui::SameLine(); 
                    ImGui::Text("%s", sit->c_str());
                    ImGui::PopID();
                }
            }
            ImGui::End();
            if(!openStates) selectedTypeForPopup = "";
        }

        //  TABELA DE DADOS 
        if(showDataTable){
            float tY = mainWinPos.y + mainWinSize.y + 10;
            float tH = (float)sh - tY - DETAILS_PANEL_HEIGHT - 30.0f;
            
            ImGui::SetNextWindowPos(ImVec2(10, tY));
            ImGui::SetNextWindowSize(ImVec2(UI_PANEL_WIDTH, tH));

            if(ImGui::Begin("Data Table", &showDataTable, ImGuiWindowFlags_NoResize)){
                if(ImGui::BeginTable("t1", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)){
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 35.0f); 
                    ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthStretch); 
                    ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthStretch); 
                    ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthStretch); 
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch); 
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();

                    for(size_t i = 0; i < cells.size(); i++){
                        ImGui::TableNextRow();

                        bool sel = (selectedCellIndex == (int)i);

                        if(sel) ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(255, 255, 0, 100));
                       
                        ImGui::TableSetColumnIndex(0);

                        if(ImGui::Selectable(std::to_string(i).c_str(), sel, ImGuiSelectableFlags_SpanAllColumns)){
                            selectedCellIndex = (selectedCellIndex == (int)i) ? -1 : (int)i; 
                        }

                        ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", cells[i].position.x);
                        ImGui::TableSetColumnIndex(2); ImGui::Text("%.1f", cells[i].position.y);
                        ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f", cells[i].position.z);
                        ImGui::TableSetColumnIndex(4); ImGui::Text("%s", cells[i].type.substr(0,4).c_str());
                        ImGui::TableSetColumnIndex(5); ImGui::Text("%s", cells[i].state.substr(0,4).c_str());
                    }

                    ImGui::EndTable();
                }
            }

            ImGui::End();
        }

        //  PAINEL DE DETALHES 
        bool showDetails = true;
        ImGui::SetNextWindowPos(ImVec2(10, (float)sh - DETAILS_PANEL_HEIGHT - 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(UI_PANEL_WIDTH, DETAILS_PANEL_HEIGHT));

        if(ImGui::Begin("Detalhes da Celula", &showDetails, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)){
            if(selectedCellIndex != -1 && selectedCellIndex < (int)cells.size()){
                auto& c = cells[selectedCellIndex];

                ImGui::Text("ID #%d", selectedCellIndex); ImGui::Separator();
                ImGui::BulletText("Tipo: %s", c.type.c_str());
                ImGui::BulletText("Estado: %s", c.state.c_str());
                ImGui::BulletText("Posicao: (%.2f, %.2f, %.2f)", c.position.x, c.position.y, c.position.z);
                ImGui::Spacing();

                if(ImGui::Button("Deselecionar", ImVec2(-FLT_MIN, 0))) selectedCellIndex = -1;
            } else{
                ImGui::TextDisabled("Selecione uma esfera ou linha.");
            }
        }

        ImGui::End();

        if(!showDetails) selectedCellIndex = -1;

        // Labels de Eixo
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImVec2 pX = worldToScreen(glm::vec3(17.5f, 0.0f, 0.0f), view, proj, sw, sh, UI_PANEL_WIDTH + 15);
        if(pX.x > -1000) dl->AddText(pX, IM_COL32(255, 0, 0, 255), "X");
        ImVec2 pY = worldToScreen(glm::vec3(0.0f, 17.5f, 0.0f), view, proj, sw, sh, UI_PANEL_WIDTH + 15);
        if(pY.x > -1000) dl->AddText(pY, IM_COL32(0, 180, 0, 255), "Y");
        ImVec2 pZ = worldToScreen(glm::vec3(0.0f, 0.0f, 17.5f), view, proj, sw, sh, UI_PANEL_WIDTH + 15);
        if(pZ.x > -1000) dl->AddText(pZ, IM_COL32(0, 0, 255, 255), "Z");

        ImGui::Render(); ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(requestSnapshot){ 
            saveSnapshot(window); 
            requestSnapshot = false; 
        }

        glfwSwapBuffers(window); glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown(); 
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true); 
    
    static bool plusPressed = false;
    static bool minusPressed = false;

    if(glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS){
        if(!plusPressed){
            if(currentSimStep < maxSimStepsFound - 1){
                currentSimStep ++; 
                parseCSV(simulationFiles[currentSimStep].path); 
            }

            plusPressed = true;
        }

    } else { 
        plusPressed = false;
    }

    if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS){
        if(!minusPressed){
            if(currentSimStep > 0){ currentSimStep --; 
                parseCSV(simulationFiles[currentSimStep].path); 
            }

            minusPressed = true;
        }
    } else {
        minusPressed = false; 
    }

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) isCameraMode = true;

    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) isCameraMode = false;

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        camera.Position = initialPos; 
        camera.Yaw = -135.0f; 
        camera.Pitch = -30.0f; 
        camera.updateCameraVectors();
    }

    if(isCameraMode){
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.Move(Camera_Movement::FORWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.Move(Camera_Movement::BACKWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.Move(Camera_Movement::LEFT, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.Move(Camera_Movement::RIGHT, deltaTime);

        float rs = 45.0f * deltaTime; 

        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.Yaw += rs;
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.Yaw -= rs;
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.Pitch += rs;
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.Pitch -= rs;

        if(camera.Pitch > 89.0f) camera.Pitch = 89.0f;
        if(camera.Pitch < -89.0f) camera.Pitch = -89.0f;
        
        camera.updateCameraVectors();
    }
}