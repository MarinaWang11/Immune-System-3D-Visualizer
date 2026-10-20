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

inline std::string cleanString(std::string str){
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c){
        return c == '\r' || c == '\n' || c == '\"' || c == '\'';
    }), str.end());

    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Converts HSL (H [0,360], S e L [0,1]) to RGB)
inline glm::vec3 hslToRgb(float h, float s, float l){
    float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;

    float r = 0.0f, g = 0.0f, b = 0.0f;
    if (h < 60.0f){ r = c; g = x; b = 0; }
    else if (h < 120.0f){ r = x; g = c; b = 0; }
    else if (h < 180.0f){ r = 0; g = c; b = x; }
    else if (h < 240.0f){ r = 0; g = x; b = c; }
    else if (h < 300.0f){ r = x; g = 0; b = c; }
    else{ r = c; g = 0; b = x; }

    return glm::vec3(r + m, g + m, b + m);
}

// Generates a Hue based in Agent's name
inline float generateAgentHue(const std::string& type){
    std::hash<std::string> hasher;
    size_t hashValue = hasher(type);
    
    const float goldenAngleConjugate = 0.618033988749895f;
    float hueFraction = std::fmod((hashValue % 1000) * goldenAngleConjugate, 1.0f);
    return hueFraction * 360.0f;
}

/*inline glm::vec3 getDefaultCellColor(const std::string& type, const std::string& state){ 
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
}*/

inline glm::vec3 getDefaultCellColor(const std::string& type, const std::string& state){
    float baseHue = generateAgentHue(type);
    
    float s = 0.80f;
    float l = 0.50f;

    if (state == "Dead" || state == "Necrotic"){
        // Grafite escuro (Lixo celular)
        return glm::vec3(0.18f, 0.18f, 0.18f); 
    } 
    else if (state == "Apoptotic"){
        // Tom roxo/cinza pálido e desbotado
        return hslToRgb(260.0f, 0.15f, 0.35f); 
    }
    else if (state == "Inactive"){
        // Opaco/apagado
        s = 0.20f;
        l = 0.35f;
    }
    else if (state == "Activated" || state == "Hyperactivated" || state == "Inflammatory" || state == "Proliferating"){
        // Alta saturação e brilho 
        s = 2.00f;
        l = 1.0f;
    } 
    else if (state == "Infected"){
        // Tom "doentio")
        baseHue = std::fmod(baseHue + 60.0f, 360.0f);
        s = 0.90f;
        l = 0.45f;
    } 
    else if (state == "Healthy" || state == "Naive"){
        s = 0.80f;
        l = 0.50f;
    }

    return hslToRgb(baseHue, s, l);
}

inline glm::vec3& getOrRegisterColor(const std::string& type, const std::string& state){
    if (colorRegistry.find(type) == colorRegistry.end() ||
        colorRegistry[type].find(state) == colorRegistry[type].end()){
                
        colorRegistry[type][state] = getDefaultCellColor(type, state);
    }
    return colorRegistry[type][state];
}

#endif