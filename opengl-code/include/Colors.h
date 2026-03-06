#ifndef COLORS_H
#define COLORS_H

#include <glm/glm.hpp>
#include <string>
#include <algorithm>

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


inline glm::vec3 getCellColor(const std::string& type, const std::string& state){
    glm::vec3 baseColor(0.6f, 0.6f, 0.6f); // Cinza padrão

    // AgentType
    if(type == "Bacteria" || type == "SAureus") 
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

    else if(type == "Antibody" || type == "IgM" || type == "IgG" || type == "AgAbComplex") 
        baseColor = glm::vec3(0.9f, 0.9f, 0.0f); // Amarelo/Dourado

    else if(type == "TissueCell") 
        baseColor = glm::vec3(0.9f, 0.7f, 0.6f); // Bege/Pele

    else if(type == "BloodVessel") 
        baseColor = glm::vec3(1.0f, 0.2f, 0.3f); // Coral

    else if(type == "TissueDamage") 
        baseColor = glm::vec3(0.2f, 0.2f, 0.2f); // Grafite

    if (type == "Cell" && state == "Healthy")
        return glm::vec3(1.0f, 0.2f, 0.3f);  //vermelha
    else if (type == "Cell" && state == "Mutant")
        return glm::vec3(0.f, 0.9f, 0.3f); // green
    else if (type == "Cell" && state == "Dead")
        return glm::vec3(0.2f, 0.2f, 0.2f); // black 
    else if (type == "Cell" && state == "Necrotic")
        return glm::vec3(0.77, 0.64, 0.52); //light brown 
    else if (type == "Nutrient")
        return glm::vec3(0.9f, 0.9f, 0.0f);        

    // AgentState
    if(state == "Dead" || state == "Necrotic" || state == "Apoptotic" || state == "Inactive"){
        return baseColor * 0.35f; // Escurece 
    }
    else if(state == "Activated" || state == "Hyperactivated" || state == "Inflammatory"){
        return glm::clamp(baseColor * 1.35f + glm::vec3(0.05f), 0.0f, 1.0f); // Vibrante
    }
    else if(state == "Infected"){
        return glm::mix(baseColor, glm::vec3(0.6f, 0.7f, 0.0f), 0.5f); // Amarelado
    }
    else if(state == "Naive" || state == "Regulatory" || state == "Healthy"){
        return glm::mix(baseColor, glm::vec3(0.6f), 0.20f); // Dessatura 
    }
    else if(state == "Healing"){
        return glm::mix(baseColor, glm::vec3(1.0f), 0.35f); // Clareia
    }

    return baseColor; // Live
}

#endif