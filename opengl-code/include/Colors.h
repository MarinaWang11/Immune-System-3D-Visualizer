#ifndef COLORS_H
#define COLORS_H

#include <glm/glm.hpp>
#include <string>
#include <algorithm>
#include <map>

inline std::map<std::string, std::map<std::string, glm::vec3>> colorRegistry;

struct SceneLighting{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

inline SceneLighting getGlobalLighting(){
    SceneLighting l;
    l.direction = glm::vec3(-0.5f, -1.0f, -0.5f);
    l.ambient   = glm::vec3(0.50f, 0.50f, 0.50f); 
    l.diffuse   = glm::vec3(0.90f, 0.90f, 0.90f); 
    l.specular  = glm::vec3(0.50f, 0.50f, 0.50f); 
    return l;
}

inline glm::vec3 getDefaultCellColor(const std::string& type, const std::string& state){ 
    glm::vec3 baseColor(0.6f, 0.6f, 0.6f); 

    if(type == "Bacteria" || type == "SAureus" || type == "Cell") 
        baseColor = glm::vec3(1.0f, 0.0f, 0.0f); // Vermelho
    else if(type == "SarsCoV2" || type == "Virus") 
        baseColor = glm::vec3(0.7f, 0.0f, 0.9f); // Roxo
    else if(type == "Neutrophil") 
        baseColor = glm::vec3(0.0f, 0.4f, 1.0f); // Azul
    else if(type == "Macrophage" || type == "MREG" || type == "AntiInflammatoryCell") 
        baseColor = glm::vec3(0.0f, 0.8f, 0.3f); // Verde
    else if(type == "DendriticCell" || type == "CH" || type == "ProInflammatoryCell") 
        baseColor = glm::vec3(1.0f, 0.6f, 0.0f); // Laranja/Ouro
    else if(type == "B" || type == "BCell" || type == "BMem") 
        baseColor = glm::vec3(0.0f, 0.7f, 0.7f); // Teal/Ciano
    else if(type == "Pl" || type == "Ps") 
        baseColor = glm::vec3(0.9f, 0.0f, 0.6f); // Magenta
    else if(type == "TCD4" || type == "TCD4Mem" || type == "TCell") 
        baseColor = glm::vec3(0.2f, 0.3f, 1.0f); // Azul Royal
    else if(type == "TCD8" || type == "TCD8Mem") 
        baseColor = glm::vec3(0.6f, 0.0f, 0.2f); // Vinho/Bordô
    else if(type == "Antibody" || type == "IgM" || type == "IgG" || type == "AgAbComplex" || type == "Nutrient") 
        baseColor = glm::vec3(0.9f, 0.9f, 0.0f); // Amarelo/Dourado
    else if(type == "TissueCell") 
        baseColor = glm::vec3(0.9f, 0.7f, 0.6f); // Bege/Pele
    else if(type == "BloodVessel") 
        baseColor = glm::vec3(1.0f, 0.2f, 0.3f); // Coral
    else if(type == "TissueDamage") 
        baseColor = glm::vec3(0.2f, 0.2f, 0.2f); // Grafite


    if(state == "Dead" || state == "Necrotic" || state == "Apoptotic" || state == "Inactive"){
        float luminance = glm::dot(baseColor, glm::vec3(0.299f, 0.587f, 0.114f)); 
        return glm::vec3(luminance * 0.35f); 
    }
    else if(state == "Activated" || state == "Hyperactivated" || state == "Inflammatory" || state == "Proliferating"){
        return glm::clamp(baseColor + glm::vec3(0.45f), 0.0f, 1.0f); 
    }
    else if(state == "Infected"){
        glm::vec3 sicklyTint = baseColor * glm::vec3(0.8f, 1.2f, 0.3f);
        return glm::clamp(sicklyTint * 0.8f, 0.0f, 1.0f); 
    }
    else if(state == "Naive" || state == "Regulatory" || state == "Healthy"){
        return glm::mix(baseColor, glm::vec3(0.0f, 1.0f, 0.0f), 0.6f); 
    }
    else if(state == "Healing"){
        return glm::mix(baseColor, glm::vec3(0.2f, 1.0f, 1.0f), 0.4f); 
    }

    return baseColor;
}

inline glm::vec3& getOrRegisterColor(const std::string& type, const std::string& state){
    if (colorRegistry.find(type) == colorRegistry.end() ||
        colorRegistry[type].find(state) == colorRegistry[type].end()){
                
        colorRegistry[type][state] = getDefaultCellColor(type, state);
    }
    return colorRegistry[type][state];
}

#endif